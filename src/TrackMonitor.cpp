#include "TrackMonitor.h"
#include "Configuration.h"
#include "Logger.h"
#include "Track.h"

TrackMonitor::TrackMonitor()
{}

void TrackMonitor::GotEvent(HitMap_t& a_hitMap, bool a_bConvertChannelToLine)
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
	std::string sLogMessage;
	if(mHitsInThisEvent.size() > 1)
	{
		sLogMessage = std::to_string(mHitsInThisEvent.size()) + "-panel track detected. Points (panel, channel): ";
		
		Track track;
		for (auto& it: mHitsInThisEvent)	
		{
			int iLine = 0;
			float fX = 0;
			if(a_bConvertChannelToLine)
			{
				iLine = Configuration::Instance().GetLineCorrespondingTo(it.first, it.second);
				fX = ChannelToX(it.first, it.second);
			}
			else
			{
				iLine = it.second;
				fX = ChannelToX(it.first, it.second, false);
			}
			sLogMessage += "(" + it.first + ", " + std::to_string(iLine)+ ") ";
			track.AddPoint(fX, PanelToZ(it.first));
		}

		FillAngleHist(track.GetAngle());
		if(track.GetNumberOfPoints() > 2)
		{	
			FillChiSquaredPerNDFHist(track.GetChiSquaredPerNDF());
		}
	//	printf("%s\n", sLogMessage.c_str());
		Logger::Instance().SetWriteCurrentMessage();
		Logger::Instance().AddMessage(sLogMessage);
	}

	m_pCanvas->Update();
	

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
	Logger::Instance().AddMessage(std::string("Chi square per NDF is ") + std::to_string(m_fChiSquaredPerNDF));

	m_pChiSquaredPerNDFHist->Fill(m_fChiSquaredPerNDF);
	m_pChiSquaredPerNDFHist->Draw();
	m_pChiSquaredPerNDFHist->SetCanExtend(TH1::kXaxis);
}

float TrackMonitor::PanelToZ(std::string a_sPanel)
{
	return Configuration::Instance().GetPanelIndex(a_sPanel) * Configuration::Instance().GetDistanceBetweenPanelsMm();
}

float TrackMonitor::ChannelToX(std::string a_sPanel, int a_iChannel, bool a_bConvertChannelToLine)
{
	int iLine = a_iChannel;
	if(a_bConvertChannelToLine)
	{
		iLine = Configuration::Instance().GetLineCorrespondingTo(a_sPanel, a_iChannel);
	}
	
	return iLine * Configuration::Instance().GetReadoutLinePitchMm();
}

void TrackMonitor::InitGraphics()
{
	printf("INITGRAPHICS\n");

	m_pCanvas = std::unique_ptr<TCanvas>(new TCanvas("TrackMonitor", "Track Monitor", 800, 600));
	m_pCanvas->Divide(2);

	m_pAngleHist = new TH1F("AngleHist" , "Polar Angle", 100, 0, 0);
	m_pAngleHist->SetFillColor(49);
	m_pAngleHist->GetXaxis()->SetTitle("Angle (radians)");
	m_pAngleHist->GetXaxis()->CenterTitle();

	m_pChiSquaredPerNDFHist = new TH1F("ChiSquarePerNDF", "Chi Square per NDF", 100, 0, 0);
	m_pChiSquaredPerNDFHist->SetFillColor(49);
}

