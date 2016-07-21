#pragma once
#include <vector>
#include <chrono>
#include "TCanvas.h"
#include "TH1.h"
#include "Types.h"

#define NO_SIGNAL_ON_PANEL -1
//using namespace std::chrono;

class PulseMonitor
{
public:	
	PulseMonitor();
	void GotEvent(SignalVector_t& a_signalMap);
	void FillPulseTimingHist(float a_fTimeDeltaNs);

private:
	void InitGraphics();
private:
	std::unique_ptr<TCanvas> m_pPulseTimingCanvas;
	TH1* m_pPulseTimingHist;
};
