#pragma once
#include <vector>
#include <chrono>
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "TF1.h"
#include "Types.h"

using namespace std::chrono;

class PanelTimingMonitor
{
public:	
	PanelTimingMonitor(std::string a_sPanelName);
	void GotEvent(int a_iChannelNum, float a_fTimeOfPulseEdge, float a_fPulseAmplitude, float a_fTriggerEdge);
private:
	void InitGraphics();
	double CrystalBall(double* x, double* par);
private:
	std::unique_ptr<TCanvas> m_pCanvas;
	std::unique_ptr<TCanvas> m_pLineTimingCanvas;
	TH1* m_pTimingHist;
	std::map<int, TH1*> m_vpLineHistograms;
	std::string m_sPanelName;
	std::unique_ptr<TF1> m_pCrystalBallFunction;
	int m_iFitCounter;
	int m_iMaxFitCounter;
};
