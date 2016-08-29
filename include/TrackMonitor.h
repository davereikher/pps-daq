#pragma once
#include <vector>
#include <chrono>
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "Types.h"
#include "Geometry.h"

#define NO_SIGNAL_ON_PANEL -1
//using namespace std::chrono;

class TrackMonitor
{
public:	
	TrackMonitor();
	void GotEvent(HitMap_t& a_hitMap, bool a_bConvertChannelToLine = true);
	float PanelIndexToZ(int a_iPanelIndex);
	int PanelToIndex(std::string a_sPanel);
	int ChannelToLine(std::string a_sPanel, int a_iChannel, bool a_bConvertChannelToLine = true);
	float LineToX(int a_iLine);
	void FillAngleHist(float a_fAngle, int a_iWeight = 1);
	void FillAngleDistanceHist(float a_fDistance, float a_fAngle);
	void FillChiSquaredPerNDFHist(float m_fChiSquaredPerNDF);
	void FillDistanceHist(float a_fDistance);
	void FillChiSquareVsAngleHist(float a_fChiSquare, float a_fAngle);
	int GetMultiplicationFactor(int a_iLine1, int a_iPanelIndex1, int a_iLine2, int a_iPanelIndex2);
	float CalculateDistance(std::vector<std::pair<int, int> > a_points);

private:
	void InitGraphics();
private:
	std::unique_ptr<TCanvas> m_pCanvasAngularDistribution;
	std::unique_ptr<TCanvas> m_pCanvas2DHist;
	std::unique_ptr<TCanvas> m_pCanvasChisquare;
	std::unique_ptr<TCanvas> m_pCanvasAvgChiSquareVsAngle;
	TH1* m_pAngleHist;
	TH1* m_pChiSquaredPerNDFHist;
	TH1* m_pDistanceHist;
	TH2* m_pDistanceAngleHist;
	TH1* m_pAngleHistFor3PointTracks;
	TH1* m_pAvgChiSquareHistVsAngle;
};
