//#include <unistd.h>
#include "RangePlotter.h"
#include "TLine.h"
#include "TAxis.h"
#include "TMarker.h"
#include "CommonUtils.h"


RangePlotter::RangePlotter(float a_fSamplingFreqGHz, float a_fMinVoltage, float a_fMaxVoltage):
m_pCanvas(new TCanvas("Canvas", "", 800, 600)),
m_colors{1, 2, 3, 4, 5, 6, 7, 8, 9, 15, 28, 46, 30, 40, 42, 38},
m_fSamplingFreqGHz(a_fSamplingFreqGHz),
m_fMinVoltage(a_fMinVoltage),
//TODO: The 0x00000FFF should be defined outside of this class!
m_fVoltageDivision((a_fMaxVoltage - a_fMinVoltage)/0x00000FFF)
{}


/**
The purpose of this function is to conveniently plot an event. Each event is plotted on a TCanvas. Divided into as many pads as there are panels. Each pad is a TMultiGraph with a TLegend, and shows the channels of the digitizer connected to the lines of that panel. The grouping into pads must not necessarily be according to panels, but in any other prefered way.

@param a_channels - a vector of a vector of samples, containing all 32 channels
@ param a_channelsToPadsAssociation - a map from std::string, which is the name of the panel (or group of channels) to be assigned to each pad to a vector if integers, which is the list of channels indices corresponding to indices of channels in the paramater a_channels to associate to a pad
@param sEventTitle - a string containing the title of the event (for example, the time stamp)
*/
void RangePlotter::PlotRanges(Channels_t& a_channels, Range_t& a_channelsToPadsAssociation, std::string sEventTitle)
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
				std::vector<float> vVoltage = TransformToVoltage(a_channels[chanIt]);
				TGraph* pGr = new TGraph(iNumOfSamples, &(CommonUtils::GenerateTimeSequence(iNumOfSamples, m_fSamplingFreqGHz)[0]), &(vVoltage[0]));
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
				std::vector<float> vTimeSeq = CommonUtils::GenerateTimeSequence(iNumOfSamples, m_fSamplingFreqGHz);	
				std::vector<float> vVoltage = TransformToVoltage(a_channels[chanIt]);
				for (int counter = 0; counter < iNumOfSamples; counter++)
				{
					m_vpGraph[i]->SetPoint(counter, vTimeSeq[counter], vVoltage[counter]);
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

std::vector<float> RangePlotter::TransformToVoltage(std::vector<float> a_vSamples)
{
	if(m_fVoltageDivision == 0)
	{
		return a_vSamples;
	}

	std::vector<float> vRes;
	vRes.resize(a_vSamples.size());
	for(int i = 0; i < a_vSamples.size(); i++)
	{
		vRes[i] = m_fMinVoltage + a_vSamples[i] * m_fVoltageDivision;
	}
	return vRes;
}

void RangePlotter::AddAnalysisMarkers(int a_iPanelIndex, SignalAnalyzer::AnalysisMarkers& a_analysisMarkers)
{
	//TODO: MEMORY LEAKS!!!
	m_pCanvas->cd(a_iPanelIndex + 1);
	printf("Adding markers. pulse threshold: %d, max x value: %f",  a_analysisMarkers.m_iPulseThreshold, m_vpMultiGraph[a_iPanelIndex]->GetXaxis()->GetXmax());
	TLine* pulseThresholdLine = new TLine(0, a_analysisMarkers.m_iPulseThreshold, m_vpMultiGraph[a_iPanelIndex]->GetXaxis()->GetXmax(), a_analysisMarkers.m_iPulseThreshold);
	pulseThresholdLine->SetLineStyle(2);
	TLine* edgeThresholdLine = new TLine(0, a_analysisMarkers.m_iEdgeThreshold, m_vpMultiGraph[a_iPanelIndex]->GetXaxis()->GetXmax(), a_analysisMarkers.m_iEdgeThreshold);
	edgeThresholdLine->SetLineStyle(2);
	
	for (auto& it: a_analysisMarkers.m_vChannelsEdgeAndMinimum)
	{
		if (std::get<0>(it).Exists())
		{
			TMarker* markerMin = new TMarker(std::get<1>(it).GetX(), std::get<1>(it).GetY(), 2);
			markerMin->Draw();
			TMarker* markerEdge = new TMarker(std::get<0>(it).GetX(), std::get<0>(it).GetY(), 3);
			markerEdge->Draw();
		}
	}

	for (auto& it: a_analysisMarkers.m_vChannelsWithPulse)
	{
		printf("*****************Channel %d has signal\n*****************", it);
	}

	pulseThresholdLine->Draw();
	edgeThresholdLine->Draw();

	m_pCanvas->Update();
}

void RangePlotter::WaitForDoubleClick()
{
	m_pCanvas->cd();
	m_pCanvas->WaitPrimitive();
}
