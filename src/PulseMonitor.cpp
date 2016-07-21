#include "PulseMonitor.h"
#include "Configuration.h"
#include "Logger.h"

PulseMonitor::PulseMonitor()
{}

void PulseMonitor::GotEvent(SignalVector_t& a_signalMap)
{
	//Use only events with two panels having primary pulses
	if(a_signalMap.size() != 2)
	{
		return;
	}

	double fLeadingEdge1 = std::get<EDGE_THRES_INDEX>(a_signalMap[0]).GetX();
	double fLeadingEdge2 = std::get<EDGE_THRES_INDEX>(a_signalMap[1]).GetX();

	FillPulseTimingHist(fLeadingEdge1 - fLeadingEdge2);
}

void PulseMonitor::FillPulseTimingHist(float a_fTimeDeltaNs)
{
	m_pPulseTimingCanvas->cd();
	m_pPulseTimingHist->Fill(a_fTimeDeltaNs);
	m_pPulseTimingHist->Draw("E1");
	m_pPulseTimingHist->SetCanExtend(TH1::kXaxis);
}
void PulseMonitor::InitGraphics()
{
	printf("INITGRAPHICS\n");

	m_pPulseTimingCanvas = std::unique_ptr<TCanvas>(new TCanvas("PulseTiming", "Distance Between Leading Edge of Pulses", 800, 600));

	m_pPulseTimingHist = new TH1F("PulseTimingHist" , "Distance Between Leading Edge of Primary Pulses", 100, 0, 0);
	m_pPulseTimingHist->SetFillColor(49);
	m_pPulseTimingHist->GetXaxis()->SetTitle("Distance (ns)");
	m_pPulseTimingHist->GetXaxis()->CenterTitle();
}
