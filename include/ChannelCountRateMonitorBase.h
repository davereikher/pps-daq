#pragma once
#include <memory>
#include <chrono>
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"

using namespace std::chrono;

class ChannelCountRateMonitorBase
{
public:
	ChannelCountRateMonitorBase(milliseconds a_periodDuration, int a_iDrawAfterNumOfEvents, std::string a_sCanvasTitle, std::string a_sRateGraphTitle, std::string a_sTimingHistTitle, std::string a_sCanvasName, std::string a_sTimingHistName);
	void GotTrigger(nanoseconds a_eventTime, int a_iChannelIndex);
	void InitGraphics();
	void AddDurationToHistogram(nanoseconds a_eventTime, int a_iChannelIndex, nanoseconds m_lastTriggerTime);
	void AddPointToRatePlot(nanoseconds a_eventTime, int a_iChannelIndex, int m_iTriggersAccumulator, nanoseconds m_periodStartTime);
	void Flush();

private:
	std::unique_ptr<TCanvas> m_pCanvas;

	TGraph* m_pRateGraph1;
	TGraph* m_pRateGraph2;
	TGraph* m_pRateGraph3;
	TGraph* m_pRateGraph4;
	TGraph* m_pRateGraph5;
	TGraph* m_pRateGraph6;
	TGraph* m_pRateGraph7;
	TGraph* m_pRateGraph8;

	milliseconds m_periodDuration;
	nanoseconds m_totalStartTime;
	nanoseconds m_periodStartTime;
	nanoseconds m_lastTriggerTime;
	int m_iTriggersAccumulator;
	int m_iDrawAfterNumOfEvents;
	int m_iNumOfEventsAccumulator;
	std::string m_sCanvasTitle;
	std::string m_sCanvasName;
	std::string m_sRateGraphTitle;
	std::string m_sTimingHistTitle;
	std::string m_sTimingHistName;
};
