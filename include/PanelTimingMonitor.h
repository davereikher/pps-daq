#pragma once
#include <vector>
#include <chrono>
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "Types.h"

using namespace std::chrono;

class PanelTimingMonitor
{
public:	
	PanelTimingMonitor(std::string a_sPanelName);
	void GotEvent(int a_iChannelNum, float a_fTimeOfPulseEdge, float a_fPulseAmplitude, float a_fTriggerEdge);
private:
	void InitGraphics();
/*	void AddToHistogram();
	void AddToSimultaneousChannelsPlot(nanoseconds a_eventTime, int a_iNumberOfSimultaneousChannels);*/
private:
	std::unique_ptr<TCanvas> m_pCanvas;
	TH1* m_pTimingHist;
	std::string m_sPanelName;
};
