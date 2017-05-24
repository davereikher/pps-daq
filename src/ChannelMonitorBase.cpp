#include "ChannelMonitorBase.h"

ChannelMonitorBase::ChannelMonitorBase(milliseconds a_periodDuration, int a_iDrawAfterNumOfEvents, std::string a_sCanvasTitle, std::string a_sRateGraphTitle, std::string a_sTimingHistTitle, std::string a_sCanvasName, std::string a_sTimingHistName):
m_periodDuration(a_periodDuration),
m_iDrawAfterNumOfEvents(a_iDrawAfterNumOfEvents),
m_sCanvasTitle(a_sCanvasTitle),
m_sCanvasName(a_sCanvasName),
m_sRateGraphTitle(a_sRateGraphTitle),
m_sTimingHistTitle(a_sTimingHistTitle),
m_sTimingHistName(a_sTimingHistName)
{}


void ChannelMonitorBase::GotTrigger(nanoseconds a_eventTime, int a_iChannelIndex)
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
	
//	m_iTriggersAccumulator[a_iChannelIndex]++;
printf("m_iTriggersAccumulator[%d]=%d", a_iChannelIndex, m_iTriggersAccumulator[a_iChannelIndex]);

	AddDurationToHistogram(a_eventTime, a_iChannelIndex, m_lastTriggerTime[a_iChannelIndex]);
	m_lastTriggerTime[a_iChannelIndex] = a_eventTime;
	m_pCanvas->Update();

/*	if (a_eventTime-m_periodStartTime[a_iChannelIndex] >= m_periodDuration)
	{
		AddPointToRatePlot(a_eventTime);
		m_iTriggersAccumulator[a_iChannelIndex] = 0;
		m_periodStartTime[a_iChannelIndex] = a_eventTime;
		
	}
	m_iNumOfEventsAccumulator[a_iChannelIndex] ++;

	if(m_iNumOfEventsAccumulator[a_iChannelIndex] == m_iDrawAfterNumOfEvents)
	{
		m_iNumOfEventsAccumulator[a_iChannelIndex] = 0;
		m_pCanvas1->Update();	
	}*/
}

void ChannelMonitorBase::InitGraphics()
{
	m_pCanvas = std::unique_ptr<TCanvas>(new TCanvas(m_sCanvasName.c_str(), m_sCanvasTitle.c_str(), 800, 600));
	m_pCanvas->Divide(2,4);
//	char sNum[10];
//	snprintf(sNum, 10, "%d", (int)(m_periodDuration.count() / (1e3)));
	for (int i = 0; i < 8; i++)
	{
		std::string name;
		
		m_vpTimingHistograms.push_back(std::unique_ptr<TH1>(new TH1F((std::string("Timing_hist") + std::to_string(i)).c_str(), m_sTimingHistTitle.c_str(), 1000, 0, 0)));

	}
/*	m_pTimingHist1 = new TH1F(m_sTimingHistName.c_str(), m_sTimingHistTitle.c_str(), 1000, 0, 0);
	m_pTimingHist2 = new TH1F(m_sTimingHistName.c_str(), m_sTimingHistTitle.c_str(), 1000, 0, 0);
	m_pTimingHist3 = new TH1F(m_sTimingHistName.c_str(), m_sTimingHistTitle.c_str(), 1000, 0, 0);
	m_pTimingHist4 = new TH1F(m_sTimingHistName.c_str(), m_sTimingHistTitle.c_str(), 1000, 0, 0);
	m_pTimingHist5 = new TH1F(m_sTimingHistName.c_str(), m_sTimingHistTitle.c_str(), 1000, 0, 0);
	m_pTimingHist6 = new TH1F(m_sTimingHistName.c_str(), m_sTimingHistTitle.c_str(), 1000, 0, 0);
	m_pTimingHist7 = new TH1F(m_sTimingHistName.c_str(), m_sTimingHistTitle.c_str(), 1000, 0, 0);
	m_pTimingHist8 = new TH1F(m_sTimingHistName.c_str(), m_sTimingHistTitle.c_str(), 1000, 0, 0);*/

}

/*void ChannelMonitorBase::AddDurationToHistogram(nanoseconds a_eventTime, int a_iChannelIndex, nanoseconds m_lastTriggerTime)
{
	if( a_iChannelIndex == 1)
	{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist1->Fill(fsec.count());
	m_pTimingHist1->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist1->GetXaxis()->CenterTitle();
	m_pTimingHist1->SetCanExtend(TH1::kXaxis);
	m_pTimingHist1->Draw();
	}
	if( a_iChannelIndex == 2)
	{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist2->Fill(fsec.count());
	m_pTimingHist2->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist2->GetXaxis()->CenterTitle();
	m_pTimingHist2->SetCanExtend(TH1::kXaxis);
	m_pTimingHist2->Draw();
	}
	if( a_iChannelIndex == 3)
	{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist3->Fill(fsec.count());
	m_pTimingHist3->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist3->GetXaxis()->CenterTitle();
	m_pTimingHist3->SetCanExtend(TH1::kXaxis);
	m_pTimingHist3->Draw();
	}
	if( a_iChannelIndex == 4)
	{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist1->Fill(fsec.count());
	m_pTimingHist1->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist1->GetXaxis()->CenterTitle();
	m_pTimingHist1->SetCanExtend(TH1::kXaxis);
	m_pTimingHist1->Draw();
	}
	if( a_iChannelIndex == 5)
	{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist5->Fill(fsec.count());
	m_pTimingHist5->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist5->GetXaxis()->CenterTitle();
	m_pTimingHist5->SetCanExtend(TH1::kXaxis);
	m_pTimingHist5->Draw();
	}

	if( a_iChannelIndex == 6)
	{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist6->Fill(fsec.count());
	m_pTimingHist6->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist6->GetXaxis()->CenterTitle();
	m_pTimingHist6->SetCanExtend(TH1::kXaxis);
	m_pTimingHist6->Draw();
	}
	if( a_iChannelIndex == 7)
	{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist7->Fill(fsec.count());
	m_pTimingHist7->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist7->GetXaxis()->CenterTitle();
	m_pTimingHist7->SetCanExtend(TH1::kXaxis);
	m_pTimingHist7->Draw();
	}
	if( a_iChannelIndex == 8)
	{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist8->Fill(fsec.count());
	m_pTimingHist8->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist8->GetXaxis()->CenterTitle();
	m_pTimingHist8->SetCanExtend(TH1::kXaxis);
	m_pTimingHist8->Draw();
	}





}*/


void ChannelMonitorBase::AddDurationToHistogram(nanoseconds a_eventTime, int a_iChannelIndex, nanoseconds m_lastTriggerTime)
{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_vpTimingHistograms[a_iChannelIndex - 1]->Fill(fsec.count());
	m_vpTimingHistograms[a_iChannelIndex - 1]->GetXaxis()->SetTitle("Time [seconds]");
	m_vpTimingHistograms[a_iChannelIndex - 1]->GetXaxis()->CenterTitle();
	m_vpTimingHistograms[a_iChannelIndex - 1]->SetCanExtend(TH1::kXaxis);
	m_vpTimingHistograms[a_iChannelIndex - 1]->Draw();
/*	if( a_iChannelIndex == 1)
	{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist1->Fill(fsec.count());
	m_pTimingHist1->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist1->GetXaxis()->CenterTitle();
	m_pTimingHist1->SetCanExtend(TH1::kXaxis);
	m_pTimingHist1->Draw();
	}
	if( a_iChannelIndex == 2)
	{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist2->Fill(fsec.count());
	m_pTimingHist2->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist2->GetXaxis()->CenterTitle();
	m_pTimingHist2->SetCanExtend(TH1::kXaxis);
	m_pTimingHist2->Draw();
	}
	if( a_iChannelIndex == 3)
	{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist3->Fill(fsec.count());
	m_pTimingHist3->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist3->GetXaxis()->CenterTitle();
	m_pTimingHist3->SetCanExtend(TH1::kXaxis);
	m_pTimingHist3->Draw();
	}
	if( a_iChannelIndex == 4)
	{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist1->Fill(fsec.count());
	m_pTimingHist1->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist1->GetXaxis()->CenterTitle();
	m_pTimingHist1->SetCanExtend(TH1::kXaxis);
	m_pTimingHist1->Draw();
	}
	if( a_iChannelIndex == 5)
	{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist5->Fill(fsec.count());
	m_pTimingHist5->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist5->GetXaxis()->CenterTitle();
	m_pTimingHist5->SetCanExtend(TH1::kXaxis);
	m_pTimingHist5->Draw();
	}

	if( a_iChannelIndex == 6)
	{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist6->Fill(fsec.count());
	m_pTimingHist6->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist6->GetXaxis()->CenterTitle();
	m_pTimingHist6->SetCanExtend(TH1::kXaxis);
	m_pTimingHist6->Draw();
	}
	if( a_iChannelIndex == 7)
	{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist7->Fill(fsec.count());
	m_pTimingHist7->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist7->GetXaxis()->CenterTitle();
	m_pTimingHist7->SetCanExtend(TH1::kXaxis);
	m_pTimingHist7->Draw();
	}
	if( a_iChannelIndex == 8)
	{
	m_pCanvas->cd(a_iChannelIndex);
	duration<float> fsec = a_eventTime - m_lastTriggerTime;
	m_pTimingHist8->Fill(fsec.count());
	m_pTimingHist8->GetXaxis()->SetTitle("Time [seconds]");
	m_pTimingHist8->GetXaxis()->CenterTitle();
	m_pTimingHist8->SetCanExtend(TH1::kXaxis);
	m_pTimingHist8->Draw();
	}*/
}

void ChannelMonitorBase::Flush()
{
	m_pCanvas->Update();
}
