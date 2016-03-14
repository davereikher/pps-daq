#include "TriggerTimingSupervisor.h"
#include "Configuration.h"

TriggerTimingSupervisor::TriggerTimingSupervisor(milliseconds a_periodDuration):
m_periodDuration(a_periodDuration),
m_lastTriggerTime(0),
m_iTriggersAccumulator(0),
m_iDrawAfterNumOfEvents(Configuration::Instance().GetNumberOfEventsToDrawAfter()),
m_iNumOfEventsAccumulator(0)
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
	m_iNumOfEventsAccumulator ++;

	if(m_iNumOfEventsAccumulator == m_iDrawAfterNumOfEvents)
	{
		m_iNumOfEventsAccumulator = 0;
		m_pCanvas->Update();	
	}
}

void TriggerTimingSupervisor::InitGraphics()
{
	//TODO: set axis labels
	m_pCanvas = std::unique_ptr<TCanvas>(new TCanvas("TriggerSupervisor", "Trigger Supervisor", 800, 600));
	m_pRateGraph = new TGraph(0);
	char sNum[10];
	snprintf(sNum, 10, "%d", (int)(m_periodDuration.count() / (1e3)));
	m_pRateGraph->SetTitle((std::string("Average trigger rate (every ") + std::string(sNum) + std::string(" seconds)")).c_str());
	m_pRateGraph->SetMarkerSize(1);
	m_pRateGraph->SetMarkerColor(4);
	m_pRateGraph->SetMarkerStyle(21);
	m_pTimingHist = new TH1F("TriggerTimingHist", "Duration Between Subsequent Triggers", 1000, 1, 500);
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

void TriggerTimingSupervisor::Flush()
{
	m_pCanvas->Update();
}
