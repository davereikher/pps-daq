#pragma once
#include <memory>
#include <chrono>
#include "TCanvas.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TH1.h"

using namespace std::chrono;

class TriggerTimingSupervisor
{
public:
	TriggerTimingSupervisor(milliseconds a_periodDuration);
	void GotTrigger(time_point<high_resolution_clock>& a_now);
	void InitGraphics();
	void AddDurationToHistogram(time_point<high_resolution_clock>& a_tp);
	void AddPointToRatePlot(time_point<high_resolution_clock>& a_tp);

private:
	std::unique_ptr<TCanvas> m_pCanvas;
	std::unique_ptr<TGraph> m_pRateGraph;
	std::unique_ptr<TH1> m_pTimingHist;
	milliseconds m_periodDuration;
	time_point<high_resolution_clock> m_totalStartTime;
	time_point<high_resolution_clock> m_periodStartTime;
	time_point<high_resolution_clock> m_lastTriggerTime;
	int m_iTriggersAccumulator;
};
