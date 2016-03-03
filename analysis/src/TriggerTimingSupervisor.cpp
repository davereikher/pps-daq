#include "TriggerTimingSupervisor.h"

TriggerTimingSupervisor::TriggerTimingSupervisor(milliseconds a_periodDuration):
m_periodDuration(a_periodDuration),
m_iTriggersAccumulator(0)
{}

void TriggerTimingSupervisor::GotTrigger(time_point<high_resolution_clock>& a_now)
{
	if(m_lastTriggerTime.time_since_epoch().count() == 0)
	{
		m_lastTriggerTime = a_now;
		m_periodStartTime = a_now;
		m_totalStartTime = a_now;
		InitGraphics();
		return;
	}

	m_iTriggersAccumulator++;

	AddDurationToHistogram(a_now);
	m_lastTriggerTime = a_now;

	if (a_now-m_periodStartTime >= m_periodDuration)
	{
		AddPointToRatePlot(a_now);
		m_iTriggersAccumulator = 0;
		m_periodStartTime = a_now;
		
	}

	m_pCanvas->Update();
}

void TriggerTimingSupervisor::InitGraphics()
{
	m_pCanvas = std::unique_ptr<TCanvas>(new TCanvas("TriggerSupervisor", "Trigger Supervisor", 800, 600));
	m_pRateGraph = std::unique_ptr<TGraph>(new TGraph(0));
	m_pRateGraph->SetMarkerSize(1);
	m_pRateGraph->SetMarkerColor(4);
	m_pRateGraph->SetMarkerStyle(21);
	m_pTimingHist = std::unique_ptr<TH1F>(new TH1F("TriggerTimingHist", "Duration Between Subsequent Triggers", 100, 0, 10000));
	m_pCanvas->Divide(1,2);
}

void TriggerTimingSupervisor::AddDurationToHistogram(time_point<high_resolution_clock>& a_tp)
{
	m_pCanvas->cd(1);
	m_pTimingHist->Fill(duration_cast<milliseconds>(a_tp - m_lastTriggerTime).count());
	m_pTimingHist->Draw();
}

void TriggerTimingSupervisor::AddPointToRatePlot(time_point<high_resolution_clock>& a_tp)
{
	m_pCanvas->cd(2);
	float fRate = float(m_iTriggersAccumulator) / duration_cast<seconds>(a_tp - m_periodStartTime).count();
	m_pRateGraph->SetPoint(m_pRateGraph->GetN(), duration_cast<seconds>(a_tp - m_totalStartTime).count(), fRate);
	m_pRateGraph->Draw("ALP");
}
