#include "ChannelCountRateMonitorBase.h"

ChannelCountRateMonitorBase::ChannelCountRateMonitorBase(milliseconds a_periodDuration, int a_iDrawAfterNumOfEvents, std::string a_sCanvasTitle, std::string a_sRateGraphTitle, std::string a_sTimingHistTitle, std::string a_sCanvasName, std::string a_sTimingHistName):
m_periodDuration(a_periodDuration),
m_iDrawAfterNumOfEvents(a_iDrawAfterNumOfEvents),
m_sCanvasTitle(a_sCanvasTitle),
m_sCanvasName(a_sCanvasName),
m_sRateGraphTitle(a_sRateGraphTitle),
m_sTimingHistTitle(a_sTimingHistTitle),
m_sTimingHistName(a_sTimingHistName)
{}
void ChannelCountRateMonitorBase::GotTrigger(nanoseconds a_eventTime, int a_iChannelIndex)
{
static int m_iTriggersAccumulator[8]={};
static int m_iChannelStart[8]={};
static int m_iNumOfEventsAccumulator[8]={};
static nanoseconds m_lastTriggerTime[8]={};
static nanoseconds m_periodStartTime[8]={};

	if(m_pCanvas == NULL)
	{		
		InitGraphics();	
		return;
	}	
	if(m_iChannelStart[a_iChannelIndex] == 0 )
	{
		m_lastTriggerTime [a_iChannelIndex] = a_eventTime;
		m_periodStartTime [a_iChannelIndex] = a_eventTime;
		m_iChannelStart[a_iChannelIndex] ++;

	}
	
	m_iTriggersAccumulator[a_iChannelIndex]++;
	printf("m_iTriggersAccumulator[%d]=%d", a_iChannelIndex, m_iTriggersAccumulator[a_iChannelIndex]);

/*	AddDurationToHistogram(a_eventTime, a_iChannelIndex, m_lastTriggerTime [a_iChannelIndex]);
	m_lastTriggerTime[a_iChannelIndex] = a_eventTime;
	m_pCanvas->Update();
*/
	if (a_eventTime-m_periodStartTime[a_iChannelIndex] >= m_periodDuration)
	{
		AddPointToRatePlot(a_eventTime, a_iChannelIndex, m_iTriggersAccumulator[a_iChannelIndex], m_periodStartTime[a_iChannelIndex]);
		m_iTriggersAccumulator[a_iChannelIndex] = 0;
		m_periodStartTime[a_iChannelIndex] = a_eventTime;
	}
	m_iNumOfEventsAccumulator[a_iChannelIndex] ++;

	if(m_iNumOfEventsAccumulator[a_iChannelIndex] == m_iDrawAfterNumOfEvents)
	{
		m_iNumOfEventsAccumulator[a_iChannelIndex] = 0;
		m_pCanvas->Update();	
	}
}

void ChannelCountRateMonitorBase::InitGraphics()
{
	m_pCanvas = std::unique_ptr<TCanvas>(new TCanvas(m_sCanvasName.c_str(), m_sCanvasTitle.c_str(), 800, 600));
	m_pRateGraph1 = new TGraph(1);
	m_pRateGraph2 = new TGraph(2);
	m_pRateGraph3 = new TGraph(3);
	m_pRateGraph4 = new TGraph(4);
	m_pRateGraph5 = new TGraph(5);
	m_pRateGraph6 = new TGraph(6);
	m_pRateGraph7 = new TGraph(7);
	m_pRateGraph8 = new TGraph(8);

	char sNum[10];
	snprintf(sNum, 10, "%d", (int)(m_periodDuration.count() / (1e3)));
	m_pRateGraph1->SetTitle((m_sRateGraphTitle + std::string(" (every ") + std::string(sNum) + std::string(" seconds)")).c_str());
	m_pRateGraph1->SetMarkerSize(1);
	m_pRateGraph1->SetMarkerColor(4);
	m_pRateGraph1->SetMarkerStyle(21);

	m_pRateGraph2->SetTitle((m_sRateGraphTitle + std::string(" (every ") + std::string(sNum) + std::string(" seconds)")).c_str());
	m_pRateGraph2->SetMarkerSize(1);
	m_pRateGraph2->SetMarkerColor(4);
	m_pRateGraph2->SetMarkerStyle(21);


	m_pRateGraph3->SetTitle((m_sRateGraphTitle + std::string(" (every ") + std::string(sNum) + std::string(" seconds)")).c_str());
	m_pRateGraph3->SetMarkerSize(1);
	m_pRateGraph3->SetMarkerColor(4);
	m_pRateGraph3->SetMarkerStyle(21);

	m_pRateGraph4->SetTitle((m_sRateGraphTitle + std::string(" (every ") + std::string(sNum) + std::string(" seconds)")).c_str());
	m_pRateGraph4->SetMarkerSize(1);
	m_pRateGraph4->SetMarkerColor(4);
	m_pRateGraph4->SetMarkerStyle(21);

	m_pRateGraph5->SetTitle((m_sRateGraphTitle + std::string(" (every ") + std::string(sNum) + std::string(" seconds)")).c_str());
	m_pRateGraph5->SetMarkerSize(1);
	m_pRateGraph5->SetMarkerColor(4);
	m_pRateGraph5->SetMarkerStyle(21);

	m_pRateGraph6->SetTitle((m_sRateGraphTitle + std::string(" (every ") + std::string(sNum) + std::string(" seconds)")).c_str());
	m_pRateGraph6->SetMarkerSize(1);
	m_pRateGraph6->SetMarkerColor(4);
	m_pRateGraph6->SetMarkerStyle(21);


	m_pRateGraph7->SetTitle((m_sRateGraphTitle + std::string(" (every ") + std::string(sNum) + std::string(" seconds)")).c_str());
	m_pRateGraph7->SetMarkerSize(1);
	m_pRateGraph7->SetMarkerColor(4);
	m_pRateGraph7->SetMarkerStyle(21);

	m_pRateGraph8->SetTitle((m_sRateGraphTitle + std::string(" (every ") + std::string(sNum) + std::string(" seconds)")).c_str());
	m_pRateGraph8->SetMarkerSize(1);
	m_pRateGraph8->SetMarkerColor(4);
	m_pRateGraph8->SetMarkerStyle(21);
//	m_pTimingHist = new TH1F(m_sTimingHistName.c_str(), m_sTimingHistTitle.c_str(), 1000, 0, 0);

	m_pCanvas->Divide(2,4);
}

/*void ChannelMonitorBase::AddDurationToHistogram(nanoseconds a_eventTime, int a_iChannelIndex, nanoseconds m_lastTriggerTime)
{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist->Fill(fsec.count());
	m_pTimingHist->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist->GetXaxis()->CenterTitle();
	m_pTimingHist->SetCanExtend(TH1::kXaxis);
	m_pTimingHist->Draw();
}*/

void ChannelCountRateMonitorBase::AddPointToRatePlot(nanoseconds a_eventTime, int a_iChannelIndex, int m_iTriggersAccumulator, nanoseconds m_periodStartTime)
{
	if( a_iChannelIndex == 1)
	{
	m_pCanvas->cd(a_iChannelIndex);
	float fRate = float(m_iTriggersAccumulator) / duration_cast<seconds>(a_eventTime - m_periodStartTime).count();
	m_pRateGraph1->SetPoint(m_pRateGraph1->GetN(), duration_cast<seconds>(a_eventTime).count(), fRate);
	m_pRateGraph1->GetXaxis()->SetTitle("Time [seconds]");
	m_pRateGraph1->GetYaxis()->SetTitle("Rate [Hz]");
	m_pRateGraph1->GetXaxis()->CenterTitle();
	m_pRateGraph1->GetYaxis()->CenterTitle();
	m_pRateGraph1->Draw("ALP");
	}
	if( a_iChannelIndex == 2)
	{
	m_pCanvas->cd(a_iChannelIndex);
	float fRate = float(m_iTriggersAccumulator) / duration_cast<seconds>(a_eventTime - m_periodStartTime).count();
	m_pRateGraph2->SetPoint(m_pRateGraph2->GetN(), duration_cast<seconds>(a_eventTime).count(), fRate);
	m_pRateGraph2->GetXaxis()->SetTitle("Time [seconds]");
	m_pRateGraph2->GetYaxis()->SetTitle("Rate [Hz]");
	m_pRateGraph2->GetXaxis()->CenterTitle();
	m_pRateGraph2->GetYaxis()->CenterTitle();
	m_pRateGraph2->Draw("ALP");
	}
	if( a_iChannelIndex == 3)
	{
	m_pCanvas->cd(a_iChannelIndex);
	float fRate = float(m_iTriggersAccumulator) / duration_cast<seconds>(a_eventTime - m_periodStartTime).count();
	m_pRateGraph3->SetPoint(m_pRateGraph3->GetN(), duration_cast<seconds>(a_eventTime).count(), fRate);
	m_pRateGraph3->GetXaxis()->SetTitle("Time [seconds]");
	m_pRateGraph3->GetYaxis()->SetTitle("Rate [Hz]");
	m_pRateGraph3->GetXaxis()->CenterTitle();
	m_pRateGraph3->GetYaxis()->CenterTitle();
	m_pRateGraph3->Draw("ALP");
	}
	if( a_iChannelIndex == 4)
	{
	m_pCanvas->cd(a_iChannelIndex);
	float fRate = float(m_iTriggersAccumulator) / duration_cast<seconds>(a_eventTime - m_periodStartTime).count();
	m_pRateGraph4->SetPoint(m_pRateGraph4->GetN(), duration_cast<seconds>(a_eventTime).count(), fRate);
	m_pRateGraph4->GetXaxis()->SetTitle("Time [seconds]");
	m_pRateGraph4->GetYaxis()->SetTitle("Rate [Hz]");
	m_pRateGraph4->GetXaxis()->CenterTitle();
	m_pRateGraph4->GetYaxis()->CenterTitle();
	m_pRateGraph4->Draw("ALP");
	}
	if( a_iChannelIndex == 5)
	{
	m_pCanvas->cd(a_iChannelIndex);
	float fRate = float(m_iTriggersAccumulator) / duration_cast<seconds>(a_eventTime - m_periodStartTime).count();
	m_pRateGraph5->SetPoint(m_pRateGraph5->GetN(), duration_cast<seconds>(a_eventTime).count(), fRate);
	m_pRateGraph5->GetXaxis()->SetTitle("Time [seconds]");
	m_pRateGraph5->GetYaxis()->SetTitle("Rate [Hz]");
	m_pRateGraph5->GetXaxis()->CenterTitle();
	m_pRateGraph5->GetYaxis()->CenterTitle();
	m_pRateGraph5->Draw("ALP");
	}
	if( a_iChannelIndex == 6)
	{
	m_pCanvas->cd(a_iChannelIndex);
	float fRate = float(m_iTriggersAccumulator) / duration_cast<seconds>(a_eventTime - m_periodStartTime).count();
	m_pRateGraph6->SetPoint(m_pRateGraph6->GetN(), duration_cast<seconds>(a_eventTime).count(), fRate);
	m_pRateGraph6->GetXaxis()->SetTitle("Time [seconds]");
	m_pRateGraph6->GetYaxis()->SetTitle("Rate [Hz]");
	m_pRateGraph6->GetXaxis()->CenterTitle();
	m_pRateGraph6->GetYaxis()->CenterTitle();
	m_pRateGraph6->Draw("ALP");
	}
	if( a_iChannelIndex == 7)
	{
	m_pCanvas->cd(a_iChannelIndex);
	float fRate = float(m_iTriggersAccumulator) / duration_cast<seconds>(a_eventTime - m_periodStartTime).count();
	m_pRateGraph7->SetPoint(m_pRateGraph7->GetN(), duration_cast<seconds>(a_eventTime).count(), fRate);
	m_pRateGraph7->GetXaxis()->SetTitle("Time [seconds]");
	m_pRateGraph7->GetYaxis()->SetTitle("Rate [Hz]");
	m_pRateGraph7->GetXaxis()->CenterTitle();
	m_pRateGraph7->GetYaxis()->CenterTitle();
	m_pRateGraph7->Draw("ALP");
	}
	if( a_iChannelIndex == 8)
	{
	m_pCanvas->cd(a_iChannelIndex);
	float fRate = float(m_iTriggersAccumulator) / duration_cast<seconds>(a_eventTime - m_periodStartTime).count();
	m_pRateGraph8->SetPoint(m_pRateGraph8->GetN(), duration_cast<seconds>(a_eventTime).count(), fRate);
	m_pRateGraph8->GetXaxis()->SetTitle("Time [seconds]");
	m_pRateGraph8->GetYaxis()->SetTitle("Rate [Hz]");
	m_pRateGraph8->GetXaxis()->CenterTitle();
	m_pRateGraph8->GetYaxis()->CenterTitle();
	m_pRateGraph8->Draw("ALP");
	}

}

void ChannelCountRateMonitorBase::Flush()
{
	m_pCanvas->Update();
}
