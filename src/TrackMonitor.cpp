#include "TrackMonitor.h"
#include "Configuration.h"
#include "Logger.h"

TrackMonitor::TrackMonitor()
{}

void TrackMonitor::GotEvent(HitMap_t& a_hitMap)
{
	//printf("Got event");
	if(m_pCanvas == NULL)
	{
		InitGraphics();
	}
	
	//printf("Panel %s, time of pulse: %f, time of trigger: %f\n", m_sPanelName.c_str(), a_fTimeOfPulseEdge, a_fTriggerEdge);
/*	m_pCanvas->cd();
	m_pTimingHist->Fill(a_fTimeOfPulseEdge - a_fTriggerEdge);
	m_pTimingHist->Draw(); 
	m_pTimingHist->SetCanExtend(TH1::kXaxis);*/
	m_pCanvas->Update();
	
/*	m_vpLineHistograms[a_iChannelNum]->Fill(a_fTimeOfPulseEdge - a_fTriggerEdge);
	m_vpLineHistograms[a_iChannelNum]->Draw();
	m_vpLineHistograms[a_iChannelNum]->SetCanExtend(TH1::kXaxis);
	m_pLineTimingCanvas->Update();*/
}

void TrackMonitor::InitGraphics()
{
	printf("INITGRAPHICS\n");

/*	m_pCanvas = std::unique_ptr<TCanvas>(new TCanvas((m_sPanelName + "_TrackMonitor").c_str(), (std::string("Panel ") + m_sPanelName  + " Timing Monitor").c_str(), 800, 600));
	m_pTimingHist = new TH1F((m_sPanelName + "TimingHist").c_str() , (std::string("Pan. ") + m_sPanelName + " Timing (Pulse-Trigger)").c_str(), 100, 0, 0);
	m_pTimingHist->SetFillColor(49);
	m_pLineTimingCanvas = std::unique_ptr<TCanvas>(new TCanvas((m_sPanelName + "_LineTimingMonitor").c_str(), (std::string("Panel ") + m_sPanelName  + " Line Timing Monitor").c_str(), 800, 600));
	std::vector<int> vChannelRange = Configuration::Instance().GetRanges()[m_sPanelName];
	int iNumberOfLines = vChannelRange.size();
	if (iNumberOfLines > 1)
	{
// determine the division of the canvas into pads according to the number of lines
		double fPads1 = ceil(sqrt(iNumberOfLines));
		double fPads2 = ceil(iNumberOfLines/fPads1);

// since the screen is wider usually, we want more pads in the horizontal direction:
		if ( fPads1 > fPads2)
		{
			m_pLineTimingCanvas->Divide((int)fPads1, (int)fPads2);	
		}
		else
		{
			m_pLineTimingCanvas->Divide((int)fPads2, (int)fPads1);
		}
	}

	for (int i = 0; i < iNumberOfLines; i++)
	{
		m_pLineTimingCanvas->cd(i + 1);
		TH1* hist = new TH1F((m_sPanelName + "_" + std::to_string(vChannelRange[i])).c_str(), (std::string("Line ") + std::to_string(vChannelRange[i])).c_str(), 100, 0, 0);
		m_vpLineHistograms[vChannelRange[i]] = hist;
		m_vpLineHistograms[vChannelRange[i]]->SetFillColor(49);
		hist->Draw();
		hist->SetCanExtend(TH1::kXaxis);
	}*/
//	m_pChannelsHist->SetStats(0);
//	m_pCanvas->Divide(1,2);

}

