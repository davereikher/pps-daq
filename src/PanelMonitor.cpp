#include "PanelMonitor.h"
#include "Logger.h"

PanelMonitor::PanelMonitor(std::string a_sPanelName):
m_sPanelName(a_sPanelName)
{}

void PanelMonitor::GotEvent(nanoseconds a_eventTime, std::vector<int> a_vChannelsWithPulse)
{	
	if(m_pCanvas == NULL)
	{
		InitGraphics();
	}
	
	AddToChannelsHistogram(a_vChannelsWithPulse);
	AddToSimultaneousChannelsPlot(a_eventTime, a_vChannelsWithPulse.size());
	m_pCanvas->Update();
}

void PanelMonitor::GotTrigger()
{
	
}

void PanelMonitor::InitGraphics()
{
	m_pCanvas = std::unique_ptr<TCanvas>(new TCanvas((m_sPanelName + "_PanelMonitor").c_str(), "Panel Monitor", 800, 600));
	m_pSimultaneousChannelGraph = new TGraph(0);
	m_pSimultaneousChannelGraph->SetTitle("Number of Channels w/ Simultaneously Detected Primary Pulses per Event");
	m_pSimultaneousChannelGraph->SetMarkerSize(1);
	m_pSimultaneousChannelGraph->SetMarkerColor(4);
	m_pSimultaneousChannelGraph->SetMarkerStyle(21);
	m_pChannelsHist = new TH1F((m_sPanelName + "ChannelHist").c_str() , "Individual Line Activity", 90, 0, 0);
	m_pChannelsHist->SetFillColor(49);
//	m_pChannelsHist->SetStats(0);
	m_pCanvas->Divide(1,2);

}

void PanelMonitor::AddToChannelsHistogram(std::vector<int>& a_vChannels)
{
	if(a_vChannels.size() == 0)
	{
		return;
	}

	std::string sLogMessage = "Num. of channels with primary pulse: " + std::to_string(a_vChannels.size()) + ". Channels ";
	m_pCanvas->cd(1);
	
	for (auto it: a_vChannels)
	{
		if (a_vChannels.size() == 1)
		{
			m_pChannelsHist->Fill(it);
		}
		sLogMessage += std::to_string(it) + " ";
	}

	Logger::Instance().SetWriteCurrentMessage();
	Logger::Instance().AddMessage(sLogMessage);
	m_pChannelsHist->GetXaxis()->SetTitle("Channel no.");
	m_pChannelsHist->GetXaxis()->CenterTitle();
	m_pChannelsHist->GetYaxis()->SetTitle("Events w/ Single Primary Pulse");
	m_pChannelsHist->GetYaxis()->CenterTitle();
	m_pChannelsHist->Draw();
	m_pChannelsHist->SetCanExtend(TH1::kXaxis);
}

void PanelMonitor::AddToSimultaneousChannelsPlot(nanoseconds a_eventTime, int a_iNumberOfSimultaneousChannels)
{
	//Don't plot 0s (when no signals are detected at all). Not interesting.
	if(a_iNumberOfSimultaneousChannels == 0)
	{
		return;
	}
	m_pCanvas->cd(2);
	m_pSimultaneousChannelGraph->SetPoint(m_pSimultaneousChannelGraph->GetN(), duration_cast<seconds>(a_eventTime).count(), a_iNumberOfSimultaneousChannels);
	m_pSimultaneousChannelGraph->GetYaxis()->SetTitle("Lines w/ Primary Pulses");
	m_pSimultaneousChannelGraph->GetXaxis()->SetTitle("Time [seconds]");
	m_pSimultaneousChannelGraph->GetXaxis()->CenterTitle();
	m_pSimultaneousChannelGraph->GetYaxis()->CenterTitle();

	m_pSimultaneousChannelGraph->Draw("AP");
}

