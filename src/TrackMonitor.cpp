#include "TrackMonitor.h"
#include "Configuration.h"
#include "Logger.h"
#include "Track.h"

TrackMonitor::TrackMonitor()
{}

void TrackMonitor::GotEvent(HitMap_t& a_hitMap, bool a_bConvertChannelToLine)
{
	if(m_pCanvasAngularDistribution == NULL)
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

//		FillAngleHist(track.GetAngle());
//		printf("Number of points in track: %d", track.GetNumberOfPoints());
		if(track.GetNumberOfPoints() == 2)
		{
			float fDistance = CalculateDistance(trackPoints);
			if (fDistance < 50)
			{
//				FillAngleHist(track.GetAngle());
			}
			
//			FillDistanceHist(trackPoints);
//			FillAngleHist(track.GetAngle());
			//printf("\n---\nOnly two points\n---\n");
			FillAngleDistanceHist(fDistance, track.GetAngle());
		}

			FillAngleHist(track.GetAngle());


		if(track.GetNumberOfPoints() > 2)
		{	
			FillChiSquareVsAngleHist(track.GetChiSquaredPerNDF(), track.GetAngle());
			FillChiSquaredPerNDFHist(track.GetChiSquaredPerNDF());
		}
	//	printf("%s\n", sLogMessage.c_str());
		Logger::Instance().SetWriteCurrentMessage();
		Logger::Instance().AddMessage(sLogMessage);
	}

	m_pCanvasAngularDistribution->Update();
	m_pCanvas2DHist->Update();
	m_pCanvasChisquare->Update();
	m_pCanvasAvgChiSquareVsAngle->Update();
	

}

void TrackMonitor::FillChiSquareVsAngleHist(float a_fChiSquare, float a_fAngle)
{
	m_pCanvasAvgChiSquareVsAngle->cd();
	int iBinIndexOfAngle = m_pAngleHistFor3PointTracks->FindFixBin(a_fAngle);
	if (iBinIndexOfAngle != (m_pAngleHistFor3PointTracks->GetXaxis()->GetLast() + 1))
	{
		int iNumberOfEntriesForAngle = m_pAngleHistFor3PointTracks->GetBinContent(iBinIndexOfAngle);
		int iAvgChiSquareBinIndex = m_pAvgChiSquareHistVsAngle->FindFixBin(a_fAngle);
		float fAverageChiSquareForAngle = m_pAvgChiSquareHistVsAngle->GetBinContent(iAvgChiSquareBinIndex);
		float fNewAverageValue = (fAverageChiSquareForAngle * iNumberOfEntriesForAngle + a_fChiSquare) / (iNumberOfEntriesForAngle + 1);
		printf("\nchisquare:%f, angle: %f, Bin: %d, num. of entries: %d, chi square bin: %d, avg chi square value: %f, new average value: %f\n", a_fChiSquare, a_fAngle, iBinIndexOfAngle, iNumberOfEntriesForAngle, iAvgChiSquareBinIndex, fAverageChiSquareForAngle, fNewAverageValue);
		m_pAvgChiSquareHistVsAngle->SetBinContent(iAvgChiSquareBinIndex, fNewAverageValue);
	}
	else
	{
		
		printf("\nNEW chisquare:%f, angle: %f\n", a_fChiSquare, a_fAngle);
		m_pAvgChiSquareHistVsAngle->Fill(a_fAngle, a_fChiSquare);
	}
	
	m_pAngleHistFor3PointTracks->Fill(a_fAngle);
//	m_pAngleHistFor3PointTracks->SetCanExtend(TH1::kXaxis);

/*	m_pAngleHistFor3PointTracks->Draw("E1");
	m_pAngleHistFor3PointTracks->SetCanExtend(TH1::kXaxis);*/
	m_pAvgChiSquareHistVsAngle->Draw("E1");
//	m_pAvgChiSquareHistVsAngle->SetCanExtend(TH1::kXaxis);
}

float TrackMonitor::CalculateDistance(std::vector<std::pair<int, int> > a_points)
{
	float fZ1 = PanelIndexToZ(a_points[0].second);
	float fX1 = LineToX(a_points[0].first);
	float fZ2 = PanelIndexToZ(a_points[1].second);
	float fX2 = LineToX(a_points[1].first);

	return Geometry::DistanceBetweenTwoPoints(Geometry::Point2D(fX1, fZ1), Geometry::Point2D(fX2, fZ2));

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
	m_pCanvasAngularDistribution->cd();
	for (int i = 0; i < a_iWeight; i++)
	{
		m_pAngleHist->Fill(a_fAngle);
	}
	m_pAngleHist->Draw("E1");
	m_pAngleHist->SetCanExtend(TH1::kXaxis);
}

void TrackMonitor::FillAngleDistanceHist(float a_fDistance, float a_fAngle)
{
	m_pCanvas2DHist->cd();
	m_pDistanceAngleHist->Fill(a_fDistance, a_fAngle);

	m_pDistanceAngleHist->Draw("LEGO1 0");
	m_pDistanceAngleHist->SetCanExtend(TH1::kXaxis);

}

/*void TrackMonitor::FillDistanceHist(float a_fDistance)
{
	m_pCanvas->cd();

	m_pDistanceHist->Fill(a_fDistance);

	m_pDistanceHist->Draw("E1");
	m_pDistanceHist->SetCanExtend(TH1::kXaxis);
}*/

void TrackMonitor::FillChiSquaredPerNDFHist(float m_fChiSquaredPerNDF)
{
	m_pCanvasChisquare->cd();
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

	m_pCanvasAngularDistribution = std::unique_ptr<TCanvas>(new TCanvas("TracksAngularDistribution", "Tracks Angular Distribution", 800, 600));
	m_pCanvas2DHist = std::unique_ptr<TCanvas>(new TCanvas("Tracks2DHist", "2-Point Tracks Angular Distribution and Distance Between Points", 800, 600));
	m_pCanvasChisquare = std::unique_ptr<TCanvas>(new TCanvas("TrackChiSquare", "Chi Square/NDF of 3 and 4 Point Tracks", 800, 600));
	m_pCanvasAvgChiSquareVsAngle = std::unique_ptr<TCanvas>(new TCanvas("AvgChiSquare", "Average Chi Square vs. Angle", 800, 600));

//	m_pCanvas->Divide(3);

	m_pAngleHist = new TH1F("AngleHist" , "Angular Distribution", 100, 0, 0);
	m_pAngleHist->SetFillColor(49);
	m_pAngleHist->GetXaxis()->SetTitle("Angle (radians)");
	m_pAngleHist->GetXaxis()->CenterTitle();

/*	m_pDistanceHist = new TH1F("AngleHist" , "Distance Between Points in 2-Point Tracks", 100, 0, 0);
	m_pDistanceHist->SetFillColor(49);
	m_pDistanceHist->GetXaxis()->SetTitle("Distance between points (mm)");
	m_pDistanceHist->GetXaxis()->CenterTitle();*/

	m_pDistanceAngleHist = new TH2F("angle_distance", "", 100, 0, 0, 100, 0, 0);
	m_pDistanceAngleHist->GetYaxis()->SetTitle("Angle (radians)");
	m_pDistanceAngleHist->GetYaxis()->CenterTitle();
	m_pDistanceAngleHist->GetYaxis()->SetTitleOffset(1.4);
	
	m_pDistanceAngleHist->GetXaxis()->SetTitle("Distance Between Points (mm)");
	m_pDistanceAngleHist->GetXaxis()->CenterTitle();
	m_pDistanceAngleHist->GetXaxis()->SetTitleOffset(1.2);

	m_pAngleHist->SetFillColor(49);

	m_pChiSquaredPerNDFHist = new TH1F("ChiSquarePerNDF", "Chi Square per NDF of 3,4 Point Tracks", 100, 0, 0);
	m_pChiSquaredPerNDFHist->GetXaxis()->SetTitle("Chi Square/NDF");
	m_pChiSquaredPerNDFHist->GetXaxis()->CenterTitle();

	m_pChiSquaredPerNDFHist->SetFillColor(49);

	m_pAvgChiSquareHistVsAngle = new TH1F("AvgChiSquarePerNDFVsAngle", "Avg. Chi Square/NDF vs. Angle", 100, -0.55, 0.55);
	m_pAvgChiSquareHistVsAngle->GetYaxis()->SetTitle("Average Chi Square/NDF");
	m_pAvgChiSquareHistVsAngle->GetYaxis()->CenterTitle();
	m_pAvgChiSquareHistVsAngle->GetXaxis()->SetTitle("Angle");
	m_pAvgChiSquareHistVsAngle->GetXaxis()->CenterTitle();
	m_pAvgChiSquareHistVsAngle->SetFillColor(49);

	m_pAngleHistFor3PointTracks = new TH1F("AngleHistFor3PointTracks", "", 100, -0.55, 0.55);
}

