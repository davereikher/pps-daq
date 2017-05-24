#pragma once
#include <memory>
#include <chrono>
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"

using namespace std::chrono;

class ChannelMonitorBase
{
public:
	ChannelMonitorBase(milliseconds a_periodDuration, int a_iDrawAfterNumOfEvents, std::string a_sCanvasTitle, std::string a_sRateGraphTitle, std::string a_sTimingHistTitle, std::string a_sCanvasName, std::string a_sTimingHistName);
	void GotTrigger(nanoseconds a_eventTime, int a_iChannelIndex);
	void InitGraphics();
	void AddDurationToHistogram(nanoseconds a_eventTime, int a_iChannelIndex, nanoseconds m_lastTriggerTime);
	void AddPointToRatePlot(nanoseconds a_eventTime);
	void Flush();

private:
	std::unique_ptr<TCanvas> m_pCanvas;

/*	TH1* m_pTimingHist1;
	TH1* m_pTimingHist2;
	TH1* m_pTimingHist3;
	TH1* m_pTimingHist4;
	TH1* m_pTimingHist5;
	TH1* m_pTimingHist6;
	TH1* m_pTimingHist7;
	TH1* m_pTimingHist8;
*/
	std::vector<std::unique_ptr<TH1>> m_vpTimingHistograms;
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
