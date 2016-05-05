#pragma once
#include <vector>
#include <chrono>
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "Types.h"

#define NO_SIGNAL_ON_PANEL -1
//using namespace std::chrono;

class TrackMonitor
{
public:	
	TrackMonitor();
	void GotEvent(HitMap_t& a_hitMap);
	float PanelToZ(std::string a_sPanel);
	float ChannelToX(std::string a_sPanel, int a_iChannel);
	void FillAngleHist(float m_fAngle);
	void FillChiSquaredPerNDFHist(float m_fChiSquaredPerNDF);

private:
	void InitGraphics();
private:
	std::unique_ptr<TCanvas> m_pCanvas;
//	std::unique_ptr<TCanvas> m_pLineTimingCanvas;
	TH1* m_pAngleHist;
	TH1* m_pChiSquaredPerNDFHist;
//	std::map<int, TH1*> m_vpLineHistograms;
//	std::string m_sPanelName;
};
