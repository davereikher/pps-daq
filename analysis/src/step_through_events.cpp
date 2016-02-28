#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TTimer.h"
#include "TSystem.h"
#include "TMultiGraph.h"
#include "TApplication.h"
#include "Analysis.h"
#include "Configuration.h"

TCanvas* g_pCanvas = NULL;
std::vector <TGraph*> g_vpGraph;
std::vector <TMultiGraph*> g_vpMultiGraph;
std::vector <TLegend*> g_vpLegends;

typedef std::map<std::string, std::vector<int> > Range_t;
typedef std::vector<std::vector<float> > Channels_t;

#define CANVAS_TITLE_MAX_SIZE 30

class TimerHandler: public TObject
{
	Bool_t HandleTimer(TTimer* timer)
	{
		gSystem->ProcessEvents();
	}
};
	
void Usage(char* a_pProcName)
{
	std::cout << "Usage: " << std::endl << "\t " << a_pProcName << " <path to configuration file> <path to root file>" << std::endl;
}


void PlotRanges(Channels_t& a_channels, Range_t& a_channelsToPadsAssociation, float a_samplingFreqGHz, std::string sEventTitle)
{	
	int colors [] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 15, 28, 46, 30, 40, 42, 38};
	
	if (!g_pCanvas)
	{
		g_pCanvas = new TCanvas("Canvas", sEventTitle.c_str(), 800, 600);
	}
	
	g_pCanvas->Clear();
	g_pCanvas->SetTitle(sEventTitle.c_str());
	printf("size of range: %d", a_channelsToPadsAssociation.size());	
	double fPads1, fPads2;
	if (a_channelsToPadsAssociation.size() > 1)
	{
// determine the division of the canvas into pads according to the size of a_channelsToPadsAssociation
		double fPads1 = ceil(sqrt(a_channelsToPadsAssociation.size()));
		double fPads2 = ceil(a_channelsToPadsAssociation.size()/fPads1);

// since the screen is wider usually, we want more pads in the horizontal direction:
		if ( fPads1 > fPads2)
		{
			g_pCanvas->Divide((int)fPads1, (int)fPads2);	
			printf("horiz pads = %d, vert pads = %d\n", (int)fPads1, (int)fPads2);
		}
		else
		{
			g_pCanvas->Divide((int)fPads2, (int)fPads1);
			printf("horiz pads = %d, vert pads = %d\n", (int)fPads2, (int)fPads1);
		}
	}
	
	int iPadCounter = 0;
	if(0 == g_vpMultiGraph.size())
	{
		for (auto& rangeIt: a_channelsToPadsAssociation)
		{	
			TMultiGraph* pMg = new TMultiGraph("mg", "mg");
			g_vpMultiGraph.push_back(pMg);

			g_pCanvas->cd(iPadCounter + 1);
			int i = 0;

			auto legend = new TLegend(0.8,0.8,1,1, "Channels");	
			g_vpLegends.push_back(legend);
			for (auto& chanIt: rangeIt.second)
			{
				printf ("Channel %d to pad %d, ", chanIt, iPadCounter);
				int iNumOfSamples = a_channels[chanIt].size();
				TGraph* pGr = new TGraph(iNumOfSamples, &(CommonUtils::GenerateTimeSequence(iNumOfSamples, a_samplingFreqGHz)[0]), &(a_channels[chanIt][0]));
				g_vpGraph.push_back(pGr);
				pGr->SetLineColor(colors[i%(sizeof(colors)/sizeof(int))]);
				std::string sMultiGraphTitle = std::string("Panel ") + rangeIt.first;
				pGr->SetName((std::string("Panel_") + rangeIt.first).c_str());
				printf("NAME: %s\n", (std::string("Channel_") + std::to_string(chanIt)).c_str());
				std::string sGraphTitle = std::string("Channel ") + std::to_string(chanIt);
				pGr->SetTitle(sGraphTitle.c_str());
				legend->AddEntry(pGr,std::to_string(chanIt).c_str(), "l");
		
				pMg->Add(pGr);
				pMg->SetTitle(sMultiGraphTitle.c_str());
				i++;
			}
			
			pMg->Draw("AC");
			legend->Draw();
			iPadCounter++;
			printf("\n");
		}
		g_pCanvas->Update();
	}
	else
	{
		int i = 0;
		for (auto& rangeIt: a_channelsToPadsAssociation)
		{
			g_pCanvas->cd(iPadCounter + 1);
			for (auto& chanIt: rangeIt.second)
			{
				printf ("Channel %d to pad %d", chanIt, iPadCounter);
				int iNumOfSamples = a_channels[chanIt].size();	
				std::vector<float> timeSeq = CommonUtils::GenerateTimeSequence(iNumOfSamples, a_samplingFreqGHz);
				for (int counter = 0; counter < iNumOfSamples; counter++)
				{
					g_vpGraph[i]->SetPoint(counter, timeSeq[counter], a_channels[chanIt][counter]);
				}
				i++;
			}

			g_vpMultiGraph[iPadCounter]->Draw("AC");
			g_vpLegends[iPadCounter]->Draw();
			iPadCounter++;
			printf("\n");
		}
		g_pCanvas->Update();
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
	
	Range_t ranges = config.GetRanges();

	Channels_t * channels = NULL;

	TFile f(sRootFileName.c_str());
	TTree* tree = (TTree*)f.Get("DigitizerEvents");
	unsigned int iTimeStamp = 0;
	tree->SetBranchAddress("TriggerTimeTag", &iTimeStamp);
	tree->SetBranchAddress("Event", &channels);

	int iNumOfEntries = tree->GetEntries();
	TimerHandler timerHandler;
	TTimer* timer = new TTimer(&timerHandler, 250, 0);
	for (int i = 0; i < iNumOfEntries; i ++)
	{
		tree->GetEntry(i);

		printf("Time stamp is %d\n", iTimeStamp);
		PlotRanges(*channels, ranges, 2.5,  std::string("Event Time Stamp ") + std::to_string(iTimeStamp)); 
		timer->TurnOn();			
		timer->Reset();
		std::cin.get();
		timer->TurnOff();

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

	if(g_pCanvas)
	{
		delete g_pCanvas;
	}
	
	for(auto& it: g_vpMultiGraph)
	{
		if(it)
		{
			delete it;
		}
	}
	
	for (auto& it: g_vpLegends)
	{
		if (it)
		{
			delete it;
		}
	}
 
	return 0;
}
