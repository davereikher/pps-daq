#include "TriggerTimingSupervisor.h"

TriggerTimingSupervisor::TriggerTimingSupervisor(milliseconds a_periodDuration):
m_periodDuration(a_periodDuration),
m_iTriggersAccumulator(0),
m_lastTriggerTime(0)
{}

void TriggerTimingSupervisor::GotTrigger(nanoseconds a_eventTime)
{
	if(m_pCanvas == NULL)
	{
		m_lastTriggerTime = a_eventTime;
		m_periodStartTime = a_eventTime;
		InitGraphics();
		return;
	}

	m_iTriggersAccumulator++;

	AddDurationToHistogram(a_eventTime);
	m_lastTriggerTime = a_eventTime;

	if (a_eventTime-m_periodStartTime >= m_periodDuration)
	{
		AddPointToRatePlot(a_eventTime);
		m_iTriggersAccumulator = 0;
		m_periodStartTime = a_eventTime;
		
	}

	m_pCanvas->Update();
}

void TriggerTimingSupervisor::InitGraphics()
{
	printf("INITIALIZEING GRAPHICS\n");
	m_pCanvas = std::unique_ptr<TCanvas>(new TCanvas("TriggerSupervisor", "Trigger Supervisor", 800, 600));
	m_pRateGraph = std::unique_ptr<TGraph>(new TGraph(0));
	m_pRateGraph->SetMarkerSize(1);
	m_pRateGraph->SetMarkerColor(4);
	m_pRateGraph->SetMarkerStyle(21);
	m_pTimingHist = std::unique_ptr<TH1F>(new TH1F("TriggerTimingHist", "Duration Between Subsequent Triggers", 100, 0, 10000));
	m_pCanvas->Divide(1,2);
}

void TriggerTimingSupervisor::AddDurationToHistogram(nanoseconds a_eventTime)
{
	m_pCanvas->cd(1);
	m_pTimingHist->Fill(duration_cast<milliseconds>(a_eventTime - m_lastTriggerTime).count());
	m_pTimingHist->Draw();
}

void TriggerTimingSupervisor::AddPointToRatePlot(nanoseconds a_eventTime)
{
	m_pCanvas->cd(2);
	float fRate = float(m_iTriggersAccumulator) / duration_cast<seconds>(a_eventTime - m_periodStartTime).count();
	m_pRateGraph->SetPoint(m_pRateGraph->GetN(), duration_cast<seconds>(a_eventTime).count(), fRate);
	m_pRateGraph->Draw("ALP");
}
