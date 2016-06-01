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
	void FillAngleDistanceHist(std::vector<std::pair<int, int> > a_points, float a_fAngle);
	void FillChiSquaredPerNDFHist(float m_fChiSquaredPerNDF);
	void FillDistanceHist(std::vector<std::pair<int, int> > a_points);
	int GetMultiplicationFactor(int a_iLine1, int a_iPanelIndex1, int a_iLine2, int a_iPanelIndex2);

private:
	void InitGraphics();
private:
	std::unique_ptr<TCanvas> m_pCanvas;
//	std::unique_ptr<TCanvas> m_pLineTimingCanvas;
	TH1* m_pAngleHist;
	TH1* m_pChiSquaredPerNDFHist;
	TH1* m_pDistanceHist;
	TH2* m_pDistanceAndleHist;

//	std::map<int, TH1*> m_vpLineHistograms;
//	std::string m_sPanelName;
};
