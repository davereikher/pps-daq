#pragma once
#include <vector>
#include <chrono>
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "Types.h"

#define NO_SIGNAL_ON_PANEL -1
typedef std::map<std::string, int> HitMap_t;
//using namespace std::chrono;

class TrackMonitor
{
public:	
	TrackMonitor();
	void GotEvent(HitMap_t& a_hitMap);
private:
	void InitGraphics();
private:
//	std::unique_ptr<TCanvas> m_pCanvas;
	/*std::unique_ptr<TCanvas> m_pLineTimingCanvas;
	TH1* m_pTimingHist;
	std::map<int, TH1*> m_vpLineHistograms;*/
//	std::string m_sPanelName;
};
