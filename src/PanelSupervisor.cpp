#include "PanelSupervisor.h"

PanelSupervisor::PanelSupervisor(std::string a_sPanelName):
m_sPanelName(a_sPanelName)
{}

void PanelSupervisor::GotEvent(nanoseconds a_eventTime, std::vector<int> a_vChannelsWithPulse)
{	
	if(m_pCanvas == NULL)
	{
		InitGraphics();
	}
	
	AddToChannelsHistogram(a_vChannelsWithPulse);
	AddToSimultaneousChannelsPlot(a_eventTime, a_vChannelsWithPulse.size());
	
}

void PanelSupervisor::InitGraphics()
{
	//TODO: set axis labels
	m_pCanvas = std::unique_ptr<TCanvas>(new TCanvas("PanelSupervisor", "Panel Supervisor", 800, 600));
	m_pSimultaneousChannelGraph = new TGraph(0);
	m_pSimultaneousChannelGraph->SetTitle("Number of Channels w/ Simultaneously Detected Pulses per Event");
	m_pSimultaneousChannelGraph->SetMarkerSize(1);
	m_pSimultaneousChannelGraph->SetMarkerColor(4);
	m_pSimultaneousChannelGraph->SetMarkerStyle(21);
	m_pChannelsHist = new TH1F("ChannelHist", "Number of Original Pulses vs Channel", 1000, 1, 5000);
	m_pCanvas->Divide(1,2);

}

void PanelSupervisor::AddToChannelsHistogram(std::vector<int>& a_vChannels)
{
	for (auto it: a_vChannels)
	{
		m_pChannelsHist->Fill(it);
	}
	m_pChannelsHist->Draw();
}

void PanelSupervisor::AddToSimultaneousChannelsPlot(nanoseconds a_eventTime, int a_iNumberOfSimultaneousChannels)
{
	m_pSimultaneousChannelGraph->SetPoint(m_pSimultaneousChannelGraph->GetN(), duration_cast<seconds>(a_eventTime).count(), a_iNumberOfSimultaneousChannels);
	m_pSimultaneousChannelGraph->Draw("ALP");
}

