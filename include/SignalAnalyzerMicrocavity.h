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
#include "DataPoint.h"
#include "SignalAnalyzer.h"


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
	};
public:
	SignalAnalyzerMicrocavity();
	void Flush();

private:
	void DoAnalysis(nanoseconds a_timeStamp, Channels_t& a_vChannels);

private:
	std::unique_ptr<TriggerTimingMonitor> m_pTriggerTimingMonitor;
};
