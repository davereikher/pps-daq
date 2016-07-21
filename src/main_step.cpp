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
	std::cout << "Usage: " << std::endl << "\t " << a_pProcName << " <path to configuration file> <path to root file> <start index>" << std::endl;
}


/**
step through the events in the root file. For each event, plot all active channels, analyse and write result to stdout
*/
int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		Usage(argv[0]);
		exit(-1);
	}

	Configuration::Instance().LoadConfiguration(argv[1]);

	std::string sRootFileName(argv[2]);
//	printf("%d digires\n\n", Configuration::GetDigitizerResolution());
//	SignalAnalyzer sigAnalyzer(2.5, -0.5, 0.5);
	SignalAnalyzer sigAnalyzer;
//	SignalAnalyzer sigAnalyzerNotNormalized;

	//The constructor of TApplication causes a segmentation violation, so we instantiate it on the heap and not delete it at the end. This is bad, but not fatal.
	TApplication* pApplication = new TApplication("app",&argc,argv);
//	RangePlotter plt(2.5, -0.5, 0.5);
	RangePlotter plt(Configuration::Instance().GetSamplingFreqGHz(), Configuration::Instance().GetVoltMin(), Configuration::Instance().GetVoltMax(),
		Configuration::Instance().GetDigitizerResolution(), "Normalized");
/*	RangePlotter pltNotNormalized(Configuration::Instance().GetSamplingFreqGHz(), Configuration::Instance().GetVoltMin(), Configuration::Instance().GetVoltMax(),
		Configuration::Instance().GetDigitizerResolution(), "NotNormalized");
*/
	
	Range_t ranges = Configuration::Instance().GetRanges();

	Channels_t * channels = NULL;

	TFile f(sRootFileName.c_str());
	TTree* tree = (TTree*)f.Get("DigitizerEvents");
	tree->SetBranchAddress("Event", &channels);

	int iNumOfEntries = tree->GetEntries();
	int iStartingIndex = std::stoi(argv[3]);
	//TimerHandler timerHandler;
	//TTimer* timer = new TTimer(&timerHandler, 250, 0);
	for (int i = iStartingIndex; i < iNumOfEntries; i ++)
	{
		tree->GetEntry(i);
		if( (*channels)[0].size() > 1)
		{
			Channels_t vNormalizedChannels = sigAnalyzer.NormalizeChannels(*channels);
		
	//		pltNotNormalized.PlotRanges(*channels, ranges, "");
			plt.PlotRanges(vNormalizedChannels, ranges, std::string("Event  ") + std::to_string(i)); 
			sigAnalyzer.FindOriginalPulseInChannelRange(vNormalizedChannels, "E", ranges["E"]);
	//		sigAnalyzerNotNormalized.FindOriginalPulseInChannelRange(*channels, ranges["A"]);
//			printf("Panel H\n");
			plt.AddAnalysisMarkers(0, sigAnalyzer.GetAnalysisMarkers());

			sigAnalyzer.FindOriginalPulseInChannelRange(vNormalizedChannels, "C", ranges["C"]);
	//		sigAnalyzerNotNormalized.FindOriginalPulseInChannelRange(*channels, ranges["A"]);
//			printf("Panel H\n");
			plt.AddAnalysisMarkers(1, sigAnalyzer.GetAnalysisMarkers());
			sigAnalyzer.FindOriginalPulseInChannelRange(vNormalizedChannels, "H", ranges["H"]);

//			printf("Panel A\n");
			plt.AddAnalysisMarkers(2, sigAnalyzer.GetAnalysisMarkers());

			sigAnalyzer.FindOriginalPulseInChannelRange(vNormalizedChannels, "B", ranges["B"]);

			plt.AddAnalysisMarkers(3, sigAnalyzer.GetAnalysisMarkers());
	//		pltNotNormalized.AddAnalysisMarkers(0, sigAnalyzerNotNormalized.GetAnalysisMarkers());
			plt.Wait();
		}
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
