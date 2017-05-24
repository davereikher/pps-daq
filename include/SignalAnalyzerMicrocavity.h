#pragma once
#include <vector>
#include <cfloat>
#include <limits.h>
#include <tuple>
#include <chrono>
#include <thread>
#include <mutex>
#include "Types.h"
#include "Queue.h"
#include "TriggerTimingMonitor.h"
#include "PanelMonitor.h"
#include "DataPoint.h"
#include "SignalAnalyzer.h"
#include "ChannelMonitor.h"
#include "ChannelCountRateMonitor.h"

#define NO_PULSE_EDGE INT_MAX
#define NO_PULSE_MINIMUM_VALUE FLT_MAX
#define NO_PULSE_MIN_VALUE_LOCATION INT_MAX

using namespace std::chrono;

class SignalAnalyzerMicrocavity: public SignalAnalyzer
{
public:
	enum AnalysisFlagsMicrocavity
	{
		ETriggerTimingMonitor = 0x00000002,
		EPanelMonitor = 0x00000004
	};
public:
	SignalAnalyzerMicrocavity();
	void Flush();
	void CountNumberOfPulses(nanoseconds a_timeStamp, Channels_t& a_vChannels);
	void CountRates(nanoseconds a_timeStamp, Channels_t& a_vChannels);
	void AfterPulse(nanoseconds a_timeStamp, Channels_t& a_vChannels);
	void AfterPulseMeasurement(nanoseconds a_timeStamp, Channels_t& a_vChannels);
	void AfterPulseMeasurement_10(nanoseconds a_timeStamp, Channels_t& a_vChannels);
	void AfterPulseMeasurement_20(nanoseconds a_timeStamp, Channels_t& a_vChannels);

private:
	void DoAnalysis(nanoseconds a_timeStamp, Channels_t& a_vChannels);

private:
	std::unique_ptr<TriggerTimingMonitor> m_pTriggerTimingMonitor;
	std::unique_ptr<ChannelMonitor> m_pChannelMonitor;
	std::unique_ptr<ChannelCountRateMonitor> m_pChannelCountRateMonitor;
	std::unique_ptr<PanelMonitor> m_pPanelMonitor;

};
