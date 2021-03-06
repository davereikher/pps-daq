//#include <unistd.h>
#include "RangePlotter.h"
#include "TLine.h"
#include "TAxis.h"
#include "TBox.h"
#include "TMarker.h"
#include "CommonUtils.h"
#include "Configuration.h"


RangePlotter::RangePlotter(float a_fSamplingFreqGHz, float a_fMinVoltage, float a_fMaxVoltage, int a_iDigitizerResolution, std::string a_sInstanceName):
m_pCanvas(new TCanvas((a_sInstanceName + "Canvas").c_str(), "", 800, 600)),
m_colors{1, 2, 3, 4, 5, 6, 7, 8, 9, 15, 28, 46, 30, 40, 42, 38},
m_fSamplingFreqGHz(a_fSamplingFreqGHz),
m_fMinVoltage(a_fMinVoltage),
m_fMaxVoltage(a_fMaxVoltage),
m_fVoltageDivision((a_fMaxVoltage - a_fMinVoltage)/a_iDigitizerResolution),
m_sInstanceName(a_sInstanceName)
{
}


void RangePlotter::MakePads(int a_iChannelsToPadsAssociationSize)
{
	if (Configuration::Instance().UseSeparateCanvasesForStep())
	{
		for (int i = 0; i < a_iChannelsToPadsAssociationSize; i++)
		{
			m_vCanvasVector.push_back(std::unique_ptr<TCanvas>(new TCanvas((m_sInstanceName + "Canvas" + std::to_string(i)).c_str(), "", 800, 600)));
		}
	}
	else
	{
		double fPads1, fPads2;
		if (a_iChannelsToPadsAssociationSize > 1)
		{
	// determine the division of the canvas into pads according to the size of a_channelsToPadsAssociation
			double fPads1 = ceil(sqrt(a_iChannelsToPadsAssociationSize));
			double fPads2 = ceil(a_iChannelsToPadsAssociationSize/fPads1);
	
	// since the screen is wider usually, we want more pads in the horizontal direction:
			if ( fPads1 > fPads2)
			{
			m_pCanvas->Divide((int)fPads1, (int)fPads2);	
			}
			else
			{
				m_pCanvas->Divide((int)fPads2, (int)fPads1);
			}
		}

	}
}

void RangePlotter::ChangePad(int a_iIndex)
{
	if(Configuration::Instance().UseSeparateCanvasesForStep())
	{
		m_vCanvasVector[a_iIndex]->cd();
	}
	else
	{
			m_pCanvas->cd(a_iIndex + 1);
	}
}

/**
The purpose of this function is to conveniently plot an event. Each event is plotted on a TCanvas. Divided into as many pads as there are panels. Each pad is a TMultiGraph with a TLegend, and shows the channels of the digitizer connected to the lines of that panel. The grouping into pads must not necessarily be according to panels, but in any other prefered way.

@param a_channels - a vector of a vector of samples, containing all 32 channels
@ param a_channelsToPadsAssociation - a map from std::string, which is the name of the panel (or group of channels) to be assigned to each pad to a vector if integers, which is the list of channels indices corresponding to indices of channels in the paramater a_channels to associate to a pad
@param sEventTitle - a string containing the title of the event (for example, the time stamp)
*/
void RangePlotter::PlotRanges(Channels_t& a_channels, Range_t& a_channelsToPadsAssociation, std::string sEventTitle)
{	
//	printf("Plotting\n");
	//m_pCanvas->Clear();
	m_pCanvas->SetTitle(sEventTitle.c_str());
	
	
	int iPadCounter = 0;
	if(0 == m_vpMultiGraph.size())
	{
		MakePads(a_channelsToPadsAssociation.size());	
		for (auto& rangeIt: a_channelsToPadsAssociation)
		{	
			TMultiGraph* pMg = new TMultiGraph();	
			m_vpMultiGraph.push_back(std::unique_ptr<TMultiGraph>(pMg));

	//		m_pCanvas->cd(iPadCounter + 1);
			ChangePad(iPadCounter);
			int i = 0;

			auto legend = new TLegend(0.8,0.8,1,1, "Channels");
			m_vpLegends.push_back(std::unique_ptr<TLegend>(legend));
			printf("printing panel %s\n", rangeIt.first.c_str());
			for (auto& chanIt: rangeIt.second)
			{
				int iNumOfSamples = a_channels[chanIt].size();
				TGraph* pGr = new TGraph(iNumOfSamples);
				std::vector<float> vTimeSeq = CommonUtils::GenerateTimeSequence(iNumOfSamples, m_fSamplingFreqGHz);
				for (int counter = 0; counter < iNumOfSamples; counter++)
				{
					pGr->SetPoint(counter, vTimeSeq[counter], TransformToVoltage(a_channels[chanIt][counter]));
				}

				m_vpGraph[chanIt] = pGr;
				pGr->SetLineColor(m_colors[i%(sizeof(m_colors)/sizeof(int))]);
				pGr->SetName((m_sInstanceName + std::string("Pan_") + rangeIt.first + std::string("chan_") + std::to_string(chanIt)).c_str());
				std::string sGraphTitle = std::string("Channel ") + std::to_string(chanIt);
				pGr->SetTitle(sGraphTitle.c_str());
				legend->AddEntry(pGr,std::to_string(chanIt).c_str(), "l");
		
				pMg->Add(pGr);
				i++;
			}

			if(Configuration::Instance().ShowTriggerInWaveformsStep())
			{
				int iNumOfSamples = a_channels[a_channels.size() - 1].size();
				m_vpGraphPrecisionTrigger = new TGraph(iNumOfSamples);
				std::vector<float> vTimeSeq = CommonUtils::GenerateTimeSequence(iNumOfSamples, m_fSamplingFreqGHz);
				for (int counter = 0; counter < iNumOfSamples; counter++)
				{
					m_vpGraphPrecisionTrigger->SetPoint(counter, vTimeSeq[counter], TransformToVoltage(a_channels[a_channels.size() - 1][counter]));
				}
				
				m_vpGraphPrecisionTrigger->SetName((m_sInstanceName + std::string("Pan_") + rangeIt.first + "_Trig").c_str());	
				m_vpGraphPrecisionTrigger->SetTitle("Trigger");
				legend->AddEntry(m_vpGraphPrecisionTrigger,"Trigger", "l");
				pMg->Add(m_vpGraphPrecisionTrigger);
			}
			
			std::string sMultiGraphTitle = std::string("Panel ") + rangeIt.first;
			pMg->SetTitle(sMultiGraphTitle.c_str());

			pMg->Draw("AL");
			pMg->GetXaxis()->SetTitle("Time [nanoseconds]");
			pMg->GetXaxis()->CenterTitle();
			pMg->GetYaxis()->SetTitle("Voltage [volts]");
			pMg->GetYaxis()->CenterTitle();

			gPad->Modified();

			pMg->SetMinimum(m_fMinVoltage);
			pMg->SetMaximum(m_fMaxVoltage);
			legend->Draw();
			iPadCounter++;
		}
		m_pCanvas->Update();
	}
	else
	{
//		printf("Plottin again\n");
		for (auto& rangeIt: a_channelsToPadsAssociation)
		{
			printf("Panel %s\n", rangeIt.first.c_str());
			m_pCanvas->cd(iPadCounter + 1);
			for (auto& chanIt: rangeIt.second)
			{
//				printf("Chanenl %d\n", chanIt);
				//TODO: num of samples is constant per run at least!
				m_vpGraph[chanIt]->SetLineWidth(1);
				int iNumOfSamples = a_channels[chanIt].size();	
				std::vector<float> vTimeSeq = CommonUtils::GenerateTimeSequence(iNumOfSamples, m_fSamplingFreqGHz);	
				for (int counter = 0; counter < iNumOfSamples; counter++)
				{
					(m_vpGraph[chanIt]->GetY())[counter] = TransformToVoltage(a_channels[chanIt][counter]);
				}

				for (int counter = 0; counter < iNumOfSamples; counter++)
				{
					(m_vpGraphPrecisionTrigger->GetY())[counter] = TransformToVoltage(a_channels[a_channels.size() - 1][counter]);
				}
	
				gPad->Modified();
			}

			m_vpMultiGraph[iPadCounter]->Draw("AC");
			m_vpLegends[iPadCounter]->Draw();
			iPadCounter++;
		}
		printf("Updating\n");
		m_pCanvas->Update();
		printf("After updating\n");
	}
	printf("done\n");
}

float RangePlotter::TransformToVoltage(float a_fSample)
{

	return  m_fMinVoltage + a_fSample * m_fVoltageDivision;
}

void RangePlotter::AddAnalysisMarkers(int a_iPanelIndex, SignalAnalyzer::AnalysisMarkers& a_analysisMarkers)
{
	m_pCanvas->cd(a_iPanelIndex + 1);
//	printf("pulse threshold: %f\n", a_analysisMarkers.GetPulseThreshold().Continuous());
	TLine* pulseThresholdLine = new TLine(0, a_analysisMarkers.GetPulseThreshold().Continuous(), m_vpMultiGraph[a_iPanelIndex]->GetXaxis()->GetXmax(), a_analysisMarkers.GetPulseThreshold().Continuous());
	pulseThresholdLine->SetLineStyle(2);
	pulseThresholdLine->SetBit(kCanDelete);
//	printf("edge threshold: %f\n", a_analysisMarkers.GetEdgeThreshold().Continuous());
	TLine* edgeThresholdLine = new TLine(0, a_analysisMarkers.GetEdgeThreshold().Continuous(), m_vpMultiGraph[a_iPanelIndex]->GetXaxis()->GetXmax(), a_analysisMarkers.GetEdgeThreshold().Continuous());
	edgeThresholdLine->SetLineStyle(2);
	edgeThresholdLine->SetBit(kCanDelete);
	
/*	int i = 0;
	for (auto& it: a_analysisMarkers.m_vChannelsEdgeAndMinimum)
	{
		int color = ((TGraph*) m_vpMultiGraph[a_iPanelIndex]->GetListOfGraphs()->At(i))->GetLineColor();
		float fYMin = m_vpMultiGraph[a_iPanelIndex]->GetYaxis()->GetXmin();
		float fYMax = m_vpMultiGraph[a_iPanelIndex]->GetYaxis()->GetXmax();

		if (std::get<EDGE_THRES_INDEX>(it).Exists())
		{	
			TMarker* markerMin = new TMarker(std::get<MIN_PULSE_INDEX>(it).GetX(), std::get<MIN_PULSE_INDEX>(it).GetY(), 22);
			markerMin->SetMarkerColor(color);
			markerMin->SetMarkerSize(2);
			markerMin->Draw();

			TBox* pulseWindow = new TBox(std::get<EDGE_THRES_INDEX>(it).GetX(), fYMin, std::get<EDGE_THRES_INDEX>(it).GetX() + a_analysisMarkers.GetExpectedPulseWidth().Continuous(), fYMax);
			pulseWindow->SetFillColor(color);
			pulseWindow->SetFillStyle(3004);
			pulseWindow->Draw();
		}
		i++;
	}
*/
	if (Configuration::Instance().TagPrimaryPulseStep())
	{
		for (auto& it: a_analysisMarkers.m_vChannelsWithPulse)
		{		
			m_vpGraph[it]->SetLineWidth(3);
		}
	}

	if (Configuration::Instance().ShowEdgeThresholdMarkerStep())
	{
		pulseThresholdLine->Draw();
	}
	
	if (Configuration::Instance().ShowPulseThresholdMarkerStep())
	{
		edgeThresholdLine->Draw();
	}

	m_pCanvas->Update();
}

void RangePlotter::Wait()
{
	m_pCanvas->cd();
	m_pCanvas->WaitPrimitive();
}
