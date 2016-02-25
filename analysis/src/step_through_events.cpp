#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TApplication.h"
#include "Analysis.h"
#include "Configuration.h"

void Usage(char* a_pProcName)
{
	std::cout << "Usage: " << std::endl << "\t " << a_pProcName << " <path to configuration file> <path to root file>" << std::endl;
}


/**
step through the events in the root file. For each event, plot all active channels, analyse and write result to stdout
*/
int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		Usage(argv[0]);
		exit(-1);
	}

	Configuration config;
	config.LoadConfiguration(argv[1]);

	std::string sRootFileName(argv[2]);

	//The constructor of TApplication causes a segmentation violation, so we instantiate it on the heap and not delete it at the end. This is bad, but not fatal.
	TApplication* pApplication = new TApplication("app",&argc,argv);
	
	std::map<std::string, std::vector<int> > mRanges = config.GetRanges();

	std::vector<std::vector<float> > vChannels;

	TFile f(sRootFileName.c_str());
	TTree* tree = (TTree*)f.Get("DigitizerEvents");
	tree->SetBranchAddress("Event", &vChannels);

	int iNumOfEntries = tree->GetEntries();
	for (int i = 0; i < iNumOfEntries; i ++)
	{
		tree->GetEntry(i);
		std::vector<float> vTimeSeq = CommonUtils::GenerateTimeSequence(vChannels[0].size(), 2.5);
		TGraph gr(vChannels[0].size(), &vChannels[0][0], &vTimeSeq[0]);
		gr.Draw("AC*");
		std::cin.get();
/*
		for (auto& it: mRanges)
		{
			std::vector<int> channels_with_pulses = SignalAnalyzer::FindOriginalPulseInChannelRange(vChannels, it.second);
			std::cout << "Panel " << it.first << " has signals on channels " << std::endl;
			for (auto& chan: channels_with_pulses)
			{
				std::cout << chan << std::endl;
			}
		}*/
	}

	return 0;
}
