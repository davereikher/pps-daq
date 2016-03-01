#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TTimer.h"
#include "TSystem.h"
#include "TApplication.h"
#include "RangePlotter.h"
#include "Configuration.h"
#include "SignalAnalyzer.h"

/*
class TimerHandler: public TObject
{
	Bool_t HandleTimer(TTimer* timer)
	{
		gSystem->ProcessEvents();
	}
};
*/	
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

	Configuration::LoadConfiguration(argv[1]);

	std::string sRootFileName(argv[2]);
//	printf("%d digires\n\n", Configuration::GetDigitizerResolution());
//	SignalAnalyzer sigAnalyzer(2.5, -0.5, 0.5);
	SignalAnalyzer sigAnalyzer(Configuration::GetSamplingFreqGHz(), Configuration::GetVoltMin(), 
		Configuration::GetVoltMax(), Configuration::GetDigitizerResolution(), Configuration::GetPulseThresholdVolts(), 
		Configuration::GetEdgeThresholdVolts(), Configuration::GetExpectedPulseWidthNs(), 
		Configuration::GetMinEdgeSeparationNs(), Configuration::GetMaxEdgeJitterNs(), 
		Configuration::GetMaxAmplitudeJitterVolts());

	//The constructor of TApplication causes a segmentation violation, so we instantiate it on the heap and not delete it at the end. This is bad, but not fatal.
	TApplication* pApplication = new TApplication("app",&argc,argv);
//	RangePlotter plt(2.5, -0.5, 0.5);
	RangePlotter plt(Configuration::GetSamplingFreqGHz(), Configuration::GetVoltMin(), Configuration::GetVoltMax(),
		Configuration::GetDigitizerResolution());

	
	Range_t ranges = Configuration::GetRanges();

	Channels_t * channels = NULL;

	TFile f(sRootFileName.c_str());
	TTree* tree = (TTree*)f.Get("DigitizerEvents");
	unsigned int iTimeStamp = 0;
	tree->SetBranchAddress("TriggerTimeTag", &iTimeStamp);
	tree->SetBranchAddress("Event", &channels);

	int iNumOfEntries = tree->GetEntries();
	//TimerHandler timerHandler;
	//TTimer* timer = new TTimer(&timerHandler, 250, 0);
	for (int i = 0; i < iNumOfEntries; i ++)
	{
		tree->GetEntry(i);

		printf("Time stamp is %d\n", iTimeStamp);
		plt.PlotRanges(*channels, ranges, std::string("Event Time Stamp ") + std::to_string(iTimeStamp)); 
		sigAnalyzer.FindOriginalPulseInChannelRange(*channels, ranges["A"]);
		plt.AddAnalysisMarkers(0, sigAnalyzer.GetAnalysisMarkers());
		plt.Wait();
/*		timer->TurnOn();			
		timer->Reset();*/
//		std::cin.get();
//		timer->TurnOff();

/*
		for (auto& it: ranges)
		{
			std::vector<int> channels_with_pulses = SignalAnalyzer::FindOriginalPulseInChannelRange(channels, it.second);
			std::cout << "Panel " << it.first << " has signals on channels " << std::endl;
			for (auto& chan: channels_with_pulses)
			{
				std::cout << chan << std::endl;
			}
		}*/
	}

	return 0;
}
