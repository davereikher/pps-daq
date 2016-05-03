#include "TrackMonitor.h"
#include "Configuration.h"
#include "Logger.h"
#include "Track.h"

TrackMonitor::TrackMonitor()
{}

void TrackMonitor::GotEvent(HitMap_t& a_hitMap)
{
	if(m_pCanvas == NULL)
	{
		InitGraphics();
	}

	std::map<std::string, int> mHitsInThisEvent;
	for (auto& it: a_hitMap)
	{
		if(it.second != NO_SIGNAL_ON_PANEL)
		{
			mHitsInThisEvent[it.first] = it.second;
		}
	}
//	printf("size of hitmap: %d", mHitsInThisEvent.size());
	std::string sLogMessage;
	if(mHitsInThisEvent.size() > 1)
	{
		sLogMessage = std::to_string(mHitsInThisEvent.size()) + "-panel track detected. Points (panel, channel): ";
//		printf("%d-panel track detected! ", mHitsInThisEvent.size());
		
		Track track;
		for (auto& it: mHitsInThisEvent)	
		{
			sLogMessage += "(" + it.first + ", " + std::to_string(Configuration::Instance().GetLineCorrespondingTo(it.first, it.second))+ ") ";
			track.AddPoint(ChannelToX(it.first, it.second), PanelToZ(it.first));
		}

		FillAngleHist(track.GetAngle());
		if(track.GetNumberOfPoints() > 2)
		{	
			FillChiSquaredPerNDFHist(track.GetChiSquaredPerNDF());
		}
		printf("%s\n", sLogMessage.c_str());
		Logger::Instance().SetWriteCurrentMessage();
		Logger::Instance().AddMessage(sLogMessage);
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

void TrackMonitor::FillAngleHist(float m_fAngle)
{
	m_pCanvas->cd(1);
	m_pAngleHist->Fill(m_fAngle);
	m_pAngleHist->Draw("E1");
	m_pAngleHist->SetCanExtend(TH1::kXaxis);
}

void TrackMonitor::FillChiSquaredPerNDFHist(float m_fChiSquaredPerNDF)
{
	m_pCanvas->cd(2);
	Logger::Instance().SetWriteCurrentMessage();
	Logger::Instance().AddMessage(std::string("Chi squared per NDF is ") + std::to_string(m_fChiSquaredPerNDF));

	m_pChiSquaredPerNDFHist->Fill(m_fChiSquaredPerNDF);
	m_pChiSquaredPerNDFHist->Draw();
	m_pChiSquaredPerNDFHist->SetCanExtend(TH1::kXaxis);
}

float TrackMonitor::PanelToZ(std::string a_sPanel)
{
	return Configuration::Instance().GetPanelIndex(a_sPanel) * Configuration::Instance().GetDistanceBetweenPanelsMm();
}

float TrackMonitor::ChannelToX(std::string a_sPanel, int a_iChannel)
{
	int line = Configuration::Instance().GetLineCorrespondingTo(a_sPanel, a_iChannel);
	return line * Configuration::Instance().GetReadoutLinePitchMm();
}

void TrackMonitor::InitGraphics()
{
	printf("INITGRAPHICS\n");

	m_pCanvas = std::unique_ptr<TCanvas>(new TCanvas("TrackMonitor", "Track Monitor", 800, 600));
	m_pCanvas->Divide(2);

	m_pAngleHist = new TH1F("AngleHist" , "Polar Angle", 1000, 0, 0);
	m_pAngleHist->SetFillColor(49);
	m_pChiSquaredPerNDFHist = new TH1F("ChiSquaredPerNDF", "Chi Squared per NDF", 100, 0, 0);
	m_pChiSquaredPerNDFHist->SetFillColor(49);
//	m_pLineTimingCanvas = std::unique_ptr<TCanvas>(new TCanvas((m_sPanelName + "_LineTimingMonitor").c_str(), (std::string("Panel ") + m_sPanelName  + " Line Timing Monitor").c_str(), 800, 600));
/*	std::vector<int> vChannelRange = Configuration::Instance().GetRanges()[m_sPanelName];
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

