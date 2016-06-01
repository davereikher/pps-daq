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
	std::vector<std::pair<int, int> > trackPoints;
	if(mHitsInThisEvent.size() > 1)
	{
		sLogMessage = std::to_string(mHitsInThisEvent.size()) + "-panel track detected. Points (panel, channel): ";
		
		Track track;
		for (auto& it: mHitsInThisEvent)	
		{
			int iLine = ChannelToLine(it.first, it.second, a_bConvertChannelToLine);
			int iPanelIndex = PanelToIndex(it.first);
			float fZ = PanelIndexToZ(iPanelIndex);
			float fX = LineToX(iLine);
			sLogMessage += "(" + it.first + ", " + std::to_string(iLine)+ ") ";
			track.AddPoint(fX, fZ);
			trackPoints.push_back(std::make_pair(iLine, iPanelIndex));
		}

		FillAngleHist(track.GetAngle());
		if(track.GetNumberOfPoints() == 2)
		{
//			FillDistanceHist(trackPoints);
//			FillAngleHist(track.GetAngle());
			FillAngleDistanceHist(trackPoints, track.GetAngle());
		}

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

int TrackMonitor::GetMultiplicationFactor(int a_iLine1, int a_iPanelIndex1, int a_iLine2, int a_iPanelIndex2)
{
	float fDeltaX = abs(a_iLine2 - a_iLine1);
	float fDeltaY = abs(a_iPanelIndex2 - a_iPanelIndex1);
	int ipFactors[] = {1, 2, 6};
	
	float fRatio = fDeltaX/fDeltaY;
	if (fRatio == 0)
	{
		return ipFactors[0];
	}

	int iMaxCount = Configuration::Instance().GetNumberOfPanels() - 1;
	for (int i = 0; i < iMaxCount; i++)
	{
		printf("Ratio is %f, ", fRatio);
		if ( (int)(fRatio * (i + 1)) > 0)
		{
			printf("mult. factor is %d\n-----------\n", ipFactors[i]);
			return ipFactors[i];
		}
		printf("\n");
	}

	return ipFactors[0];
}

void TrackMonitor::FillAngleHist(float a_fAngle, int a_iWeight)
{
	m_pCanvas->cd(1);
	for (int i = 0; i < a_iWeight; i++)
	{
		m_pAngleHist->Fill(a_fAngle);
	}
	m_pAngleHist->Draw("E1");
	m_pAngleHist->SetCanExtend(TH1::kXaxis);
}

void TrackMonitor::FillAngleDistanceHist(std::vector<std::pair<int, int> > a_points, float a_fAngle)
{
	m_pCanvas->cd(3);
	float fZ1 = PanelIndexToZ(a_points[0].second);
	float fX1 = LineToX(a_points[0].first);
	float fZ2 = PanelIndexToZ(a_points[1].second);
	float fX2 = LineToX(a_points[1].first);

	float distance = Geometry::DistanceBetweenTwoPoints(Geometry::Point2D(fX1, fZ1), Geometry::Point2D(fX2, fZ2));
	printf("distance between (%f, %f) and (%f, %f): %f\n", fX1, fZ1, fX2, fZ2, distance);
	m_pDistanceAndleHist->Fill(distance, a_fAngle);

	m_pDistanceAndleHist->Draw("LEGO1 0");
	m_pDistanceAndleHist->SetCanExtend(TH1::kXaxis);

}

void TrackMonitor::FillDistanceHist(std::vector<std::pair<int, int> > a_points)
{
	m_pCanvas->cd(3);
	float fZ1 = PanelIndexToZ(a_points[0].second);
	float fX1 = LineToX(a_points[0].first);
	float fZ2 = PanelIndexToZ(a_points[1].second);
	float fX2 = LineToX(a_points[1].first);

	float distance = Geometry::DistanceBetweenTwoPoints(Geometry::Point2D(fX1, fZ1), Geometry::Point2D(fX2, fZ2));
	printf("distance between (%f, %f) and (%f, %f): %f\n", fX1, fZ1, fX2, fZ2, distance);
	int iMultFactor = GetMultiplicationFactor(a_points[0].first, a_points[0].second, a_points[1].first, a_points[1].second);
	for (int i = 0; i < iMultFactor; i++)
	{	
		m_pDistanceHist->Fill(distance);
	}

	m_pDistanceHist->Draw("E1");
	m_pDistanceHist->SetCanExtend(TH1::kXaxis);
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

int TrackMonitor::PanelToIndex(std::string a_sPanel)
{
	return Configuration::Instance().GetPanelIndex(a_sPanel);
}

float TrackMonitor::PanelIndexToZ(int a_iPanelIndex)
{
	return a_iPanelIndex * Configuration::Instance().GetDistanceBetweenPanelsMm();
}

int TrackMonitor::ChannelToLine(std::string a_sPanel, int a_iChannel, bool a_bConvertChannelToLine)
{
	int iLine = a_iChannel;
	if(a_bConvertChannelToLine)
	{
		iLine = Configuration::Instance().GetLineCorrespondingTo(a_sPanel, a_iChannel);
	}
	return iLine;
}

float TrackMonitor::LineToX(int a_iLine)
{
	
	return a_iLine * Configuration::Instance().GetReadoutLinePitchMm();
}

void TrackMonitor::InitGraphics()
{
	printf("INITGRAPHICS\n");

	m_pCanvas = std::unique_ptr<TCanvas>(new TCanvas("TrackMonitor", "Track Monitor", 800, 600));
	m_pCanvas->Divide(3);

	m_pAngleHist = new TH1F("AngleHist" , "Polar Angle", 100, 0, 0);
	m_pAngleHist->SetFillColor(49);
	m_pAngleHist->GetXaxis()->SetTitle("Angle (radians)");
	m_pAngleHist->GetXaxis()->CenterTitle();

/*	m_pDistanceHist = new TH1F("AngleHist" , "Distance Between Points in 2-Point Tracks", 100, 0, 0);
	m_pDistanceHist->SetFillColor(49);
	m_pDistanceHist->GetXaxis()->SetTitle("Distance between points (mm)");
	m_pDistanceHist->GetXaxis()->CenterTitle();*/

	m_pDistanceAndleHist = new TH2F("angle_distance", "Track Angle by Distance Between Points on Track", 100, 0, 0, 100, 0, 0);
	m_pAngleHist->SetFillColor(49);

	m_pChiSquaredPerNDFHist = new TH1F("ChiSquarePerNDF", "Chi Square per NDF", 100, 0, 0);
	m_pChiSquaredPerNDFHist->SetFillColor(49);
}

