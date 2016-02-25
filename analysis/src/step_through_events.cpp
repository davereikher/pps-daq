#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TApplication.h"
#include "Analysis.h"
#include "Configuration.h"

TCanvas* g_pCanvas = NULL;
typedef std::map<std::string, std::vector<int> > Range_t;
typedef std::vector<std::vector<float> > Channels_t;

void Usage(char* a_pProcName)
{
	std::cout << "Usage: " << std::endl << "\t " << a_pProcName << " <path to configuration file> <path to root file>" << std::endl;
}


void PlotRanges(Channels_t& a_channels, Range_t& a_channelsToPadsAssociation, float a_samplingFreqGHz)
{
	if (!g_pCanvas)
	{
		g_pCanvas = new TCanvas("Canvas", "Selected Channels", 800, 600);
	}
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
	
	Range_t mRanges = config.GetRanges();

	Channels_t * vChannels = NULL;

	TFile f(sRootFileName.c_str());
	TTree* tree = (TTree*)f.Get("DigitizerEvents");
	int iTimeStamp;
	tree->SetBranchAddress("EventInfo", &iTimeStamp);
	tree->SetBranchAddress("Event", &vChannels);

	int iNumOfEntries = tree->GetEntries();
	TCanvas canvas("canvas", "Title", 800, 600);
//	canvas->cd(1);
	for (int i = 0; i < iNumOfEntries; i ++)
	{
		tree->GetEntry(i);
		std::vector<float> vTimeSeq = CommonUtils::GenerateTimeSequence((*vChannels)[0].size(), 2.5);
		TGraph gr((*vChannels)[0].size(),  &(vTimeSeq[0]), &((*vChannels)[0][0]));
		gr.Draw("ACP");
		canvas.Update();
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

cleanup:
	if(g_pCanvas)
	{
		delete g_pCanvas;
	}
	return 0;
}
