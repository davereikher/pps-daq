#pragma once
#include <memory>
#include <chrono>
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"

using namespace std::chrono;

class TriggerTimingMonitor
{
public:
	TriggerTimingMonitor(milliseconds a_periodDuration);
	void GotTrigger(nanoseconds a_eventTime);
	void InitGraphics();
	void AddDurationToHistogram(nanoseconds a_eventTime);
	void AddPointToRatePlot(nanoseconds a_eventTime);
	void Flush();

private:
	std::unique_ptr<TCanvas> m_pCanvas;
	TGraph* m_pRateGraph;
	TH1* m_pTimingHist;
	milliseconds m_periodDuration;
	nanoseconds m_totalStartTime;
	nanoseconds m_periodStartTime;
	nanoseconds m_lastTriggerTime;
	int m_iTriggersAccumulator;
	int m_iDrawAfterNumOfEvents;
	int m_iNumOfEventsAccumulator;
};
