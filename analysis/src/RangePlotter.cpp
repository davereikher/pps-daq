#include "RangePlotter.h"
#include "CommonUtils.h"

RangePlotter::RangePlotter():
m_pCanvas(new TCanvas("Canvas", "", 800, 600)),
m_colors{1, 2, 3, 4, 5, 6, 7, 8, 9, 15, 28, 46, 30, 40, 42, 38}
{}

void RangePlotter::PlotRanges(Channels_t& a_channels, Range_t& a_channelsToPadsAssociation, float a_samplingFreqGHz, std::string sEventTitle)
{		
	m_pCanvas->Clear();
	m_pCanvas->SetTitle(sEventTitle.c_str());
	
	double fPads1, fPads2;
	if (a_channelsToPadsAssociation.size() > 1)
	{
// determine the division of the canvas into pads according to the size of a_channelsToPadsAssociation
		double fPads1 = ceil(sqrt(a_channelsToPadsAssociation.size()));
		double fPads2 = ceil(a_channelsToPadsAssociation.size()/fPads1);

// since the screen is wider usually, we want more pads in the horizontal direction:
		if ( fPads1 > fPads2)
		{
			m_pCanvas->Divide((int)fPads1, (int)fPads2);	
			printf("horiz pads = %d, vert pads = %d\n", (int)fPads1, (int)fPads2);
		}
		else
		{
			m_pCanvas->Divide((int)fPads2, (int)fPads1);
			printf("horiz pads = %d, vert pads = %d\n", (int)fPads2, (int)fPads1);
		}
	}
	
	int iPadCounter = 0;
	if(0 == m_vpMultiGraph.size())
	{
		for (auto& rangeIt: a_channelsToPadsAssociation)
		{	
			TMultiGraph* pMg = new TMultiGraph("mg", "mg");
			m_vpMultiGraph.push_back(std::unique_ptr<TMultiGraph>(pMg));

			m_pCanvas->cd(iPadCounter + 1);
			int i = 0;

			auto legend = new TLegend(0.8,0.8,1,1, "Channels");	
			m_vpLegends.push_back(std::unique_ptr<TLegend>(legend));
			for (auto& chanIt: rangeIt.second)
			{
				printf ("Channel %d to pad %d, ", chanIt, iPadCounter);
				int iNumOfSamples = a_channels[chanIt].size();
				TGraph* pGr = new TGraph(iNumOfSamples, &(CommonUtils::GenerateTimeSequence(iNumOfSamples, a_samplingFreqGHz)[0]), &(a_channels[chanIt][0]));
				m_vpGraph.push_back(pGr);
				pGr->SetLineColor(m_colors[i%(sizeof(m_colors)/sizeof(int))]);
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
		m_pCanvas->Update();
	}
	else
	{
		int i = 0;
		for (auto& rangeIt: a_channelsToPadsAssociation)
		{
			m_pCanvas->cd(iPadCounter + 1);
			for (auto& chanIt: rangeIt.second)
			{
				printf ("Channel %d to pad %d", chanIt, iPadCounter);
				int iNumOfSamples = a_channels[chanIt].size();	
				std::vector<float> timeSeq = CommonUtils::GenerateTimeSequence(iNumOfSamples, a_samplingFreqGHz);
				for (int counter = 0; counter < iNumOfSamples; counter++)
				{
					m_vpGraph[i]->SetPoint(counter, timeSeq[counter], a_channels[chanIt][counter]);
				}
				i++;
			}

			m_vpMultiGraph[iPadCounter]->Draw("AC");
			m_vpLegends[iPadCounter]->Draw();
			iPadCounter++;
			printf("\n");
		}
		m_pCanvas->Update();
	}
}

