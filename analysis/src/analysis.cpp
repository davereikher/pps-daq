#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TApplication.h"
#include "SignalAnalyzer.h"
#include "CommonUtils.h"
#include "Configuration.h"

void Usage(char* a_pProcName)
{
	std::cout << "Usage: " << std::endl << "\t " << a_pProcName << " <path to configuration file> <path to root file>" << std::endl;
	std::vector<float> vec = CommonUtils::GenerateTimeSequence(5);
}


int main(int argc, char* argv[])
{
	Configuration config;
	config.LoadConfiguration(argv[1]);

	std::string sRootFileName(argv[2]);
	SignalAnalyzer sigAnalyzer(0, 0, 0);

	//The constructor of TApplication causes a segmentation violation, so we instantiate it on the heap and not delete it at the end. This is bad, but not fatal.
	TApplication* pApplication = new TApplication("app",&argc,argv);
	
	if (argc < 2)
	{
		Usage(argv[0]);
		exit(-1);
	}
	
	std::map<std::string, std::vector<int> > mRanges = config.GetRanges();

	std::vector<std::vector<float> > vChannels;

	TFile f(sRootFileName.c_str());
	TTree* tree = (TTree*)f.Get("DigitizerEvents");
	tree->SetBranchAddress("Events", &vChannels);

	int iNumOfEntries = tree->GetEntries();
	for (int i = 0; i < iNumOfEntries; i ++)
	{
		tree->GetEntry(i);
		for (auto& it: mRanges)
		{
			sigAnalyzer.FindOriginalPulseInChannelRange(vChannels, it.second);
			std::cout << "Panel " << it.first << " has signals on channels " << std::endl;
			for (auto& chan: sigAnalyzer.GetAnalysisMarkers().m_vChannelsWithPulse)
			{
				std::cout << chan << std::endl;
			}
		}
	}

	return 0;
}
