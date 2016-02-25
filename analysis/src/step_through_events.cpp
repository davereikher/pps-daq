#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TApplication.h"
#include "Analysis.h"
#include "Configuration.h"

TCanvas* g_pCanvas = NULL;
std::vector <TGraph*> g_vpGraph;
std::vector <TMultiGraph*> g_vpMultiGraph;

typedef std::map<std::string, std::vector<int> > Range_t;
typedef std::vector<std::vector<float> > Channels_t;
	
void Usage(char* a_pProcName)
{
	std::cout << "Usage: " << std::endl << "\t " << a_pProcName << " <path to configuration file> <path to root file>" << std::endl;
}


void PlotRanges(Channels_t& a_channels, Range_t& a_channelsToPadsAssociation, float a_samplingFreqGHz)
{	
	int colors [] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 15, 28, 46, 30, 40, 42, 38};

	if (!g_pCanvas)
	{
		g_pCanvas = new TCanvas("Canvas", "Selected Channels", 800, 600);
	}
	
	g_pCanvas->Clear();
	
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

	int iPadCounter = 0;
	if(0 == g_vpMultiGraph.size())
	{
		for (auto& rangeIt: a_channelsToPadsAssociation)
		{	
			TMultiGraph* pMg = new TMultiGraph("mg", "mg");
			g_vpMultiGraph.push_back(pMg);

			g_pCanvas->cd(iPadCounter + 1);
			int i = 0;
			for (auto& chanIt: rangeIt.second)
			{
				printf ("Channel %d to pad %d, ", chanIt, iPadCounter);
				int iNumOfSamples = a_channels[chanIt].size();
				TGraph* pGr = new TGraph(iNumOfSamples, &(CommonUtils::GenerateTimeSequence(iNumOfSamples, a_samplingFreqGHz)[0]), &(a_channels[chanIt][0]));
				g_vpGraph.push_back(pGr);
				pGr->SetLineColor(colors[i%(sizeof(colors)/sizeof(int))]);
				/*char sGraphName[100];
				snprintf(100, sGraphName, "Panel*/
				pGr->SetName((std::string("Panel_") + rangeIt.first).c_str());
				pGr->SetTitle((std::string("Panel ") + rangeIt.first).c_str());
				pMg->Add(pGr);
				i++;
			}
			pMg->Draw("AC");
			iPadCounter++;
			printf("\n");
		}
		g_pCanvas->BuildLegend();
//		g_pCanvas->Draw();
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
				g_vpGraph[i]->DrawGraph(iNumOfSamples, &(CommonUtils::GenerateTimeSequence(iNumOfSamples, a_samplingFreqGHz)[0]), &(a_channels[chanIt][0]));
				i++;
			}

			g_vpMultiGraph[iPadCounter]->Draw("AC");
			iPadCounter++;
			printf("\n");
		}
//		g_pCanvas->BuildLegend();
		g_pCanvas->Update();
		g_pCanvas->Modified();
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
	int iTimeStamp;
//	tree->SetBranchAddress("EventInfo", &iTimeStamp);
	tree->SetBranchAddress("Event", &channels);

	int iNumOfEntries = tree->GetEntries();
//	TCanvas canvas("canvas", "Title", 800, 600);
//	canvas->cd(1);
	for (int i = 0; i < iNumOfEntries; i ++)
	{
		tree->GetEntry(i);
/*		std::vector<float> vTimeSeq = CommonUtils::GenerateTimeSequence((*channels)[0].size(), 2.5);
		TGraph gr((*channels)[0].size(),  &(vTimeSeq[0]), &((*channels)[0][0]));
		gr.Draw("ACP");
		canvas.Update();*/
		PlotRanges(*channels, ranges, 2.5);
		std::cin.get();

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

//cleanup:
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
 
	return 0;
}
