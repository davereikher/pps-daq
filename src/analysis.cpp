#include <iostream>
#include "keyb.h"
#include "TFile.h"
#include "TTree.h"
#include "TApplication.h"
#include "SignalAnalyzer.h"
#include "CommonUtils.h"
#include "Configuration.h"
#include "Types.h"

void Usage(char* a_pProcName)
{
	std::cout << "Usage: " << std::endl << "\t " << a_pProcName << " <path to configuration file> <path to root file>" << std::endl;
	std::vector<float> vec = CommonUtils::GenerateTimeSequence(5);
}


int main(int argc, char* argv[])
{
	Configuration::LoadConfiguration(argv[1]);

	std::string sRootFileName(argv[2]);
	SignalAnalyzer sigAnalyzer(Configuration::GetSamplingFreqGHz(), Configuration::GetVoltMin(), 
		Configuration::GetVoltMax(), Configuration::GetDigitizerResolution(), Configuration::GetPulseThresholdVolts(), 
		Configuration::GetEdgeThresholdVolts(), Configuration::GetExpectedPulseWidthNs(), 
		Configuration::GetMinEdgeSeparationNs(), Configuration::GetMaxEdgeJitterNs(), 
		Configuration::GetMaxAmplitudeJitterVolts());

	sigAnalyzer.SetFlags(SignalAnalyzer::ETriggerTimingSupervisor);


	//The constructor of TApplication causes a segmentation violation, so we instantiate it on the heap and not delete it at the end. This is bad, but not fatal.
//	TApplication* pApplication = new TApplication("app",&argc,argv);
	
	if (argc < 2)
	{
		Usage(argv[0]);
		exit(-1);
	}
	
	Range_t mRanges = Configuration::GetRanges();

	Channels_t* vChannels;
	unsigned int iArrivalTimeMSB = 0;
	unsigned int iArrivalTimeLSB = 0;

	TFile f(sRootFileName.c_str());
	TTree* tree = (TTree*)f.Get("DigitizerEvents");
	tree->SetBranchAddress("Event", &vChannels);
	tree->SetBranchAddress("ArrivalTimeMSB", &iArrivalTimeMSB);	
	tree->SetBranchAddress("ArrivalTimeLSB", &iArrivalTimeLSB);

	int iNumOfEntries = tree->GetEntries();
	for (int i = 0; i < iNumOfEntries; i ++)
	{
		tree->GetEntry(i);
		for (auto& it: mRanges)
		{
			int64_t iTimeStampNano = (((uint64_t)iArrivalTimeMSB) << 32) & iArrivalTimeLSB;
			sigAnalyzer.Analyze(nanoseconds(iTimeStampNano), *vChannels);
//			sigAnalyzer.FindOriginalPulseInChannelRange(vChannels, it.second);
//			std::cout << "Panel " << it.first << " has signals on channels " << std::endl;
/*			for (auto& chan: sigAnalyzer.GetAnalysisMarkers().m_vChannelsWithPulse)
			{
				std::cout << chan << std::endl;
			}*/
		}
	}
	int c = 0;
	printf("press q to quit...\n");
	do
	{
		c = getch();
	} while(c != 'q');
	
	return 0;
}
