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
#include "PanelMonitor.h"
#include "TriggerTimingMonitor.h"
#include "PanelMonitor.h"
#include "PanelTimingMonitor.h"
#include "TrackMonitor.h"
#include "PulseMonitor.h"
#include "PanelDegradationMonitor.h"
#include "DataPoint.h"
#include "SignalAnalyzer.h"


#define NO_PULSE_EDGE INT_MAX
#define NO_PULSE_MINIMUM_VALUE FLT_MAX
#define NO_PULSE_MIN_VALUE_LOCATION INT_MAX

using namespace std::chrono;

class SignalAnalyzerTracker: public SignalAnalyzer
{
public:
	enum AnalysisFlagsTracker
	{
		ETriggerTimingMonitor = 0x00000002,
		EPanelHitMonitor = 0x00000004,
		EPanelTimingMonitor = 0x00000008,
		ETrackMonitor = 0x00000010,
		ECountPanelsWithPrimaryPulse = 0x00000020,
		EPanelDegradationMonitor = 0x00000040,
		EPulseMonitor = 0x00000080
	};
public:
	SignalAnalyzerTracker();
	void AnalyzeTrack(HitMap_t& a_panelAndLine);
	int GetLastNumberOfPanelsWithPrimaryPulse();
	void Flush();

private:
	void DoAnalysis(nanoseconds a_timeStamp, Channels_t& a_vChannels);

private:
	Range_t m_vRanges;
	std::unique_ptr<TriggerTimingMonitor> m_pTriggerTimingMonitor;
	std::vector<std::unique_ptr<PanelMonitor> > m_vpPanelMonitors;
	std::vector<std::unique_ptr<PanelDegradationMonitor> > m_vpPanelDegradationMonitors;
	std::vector<std::unique_ptr<PanelTimingMonitor> > m_vpPanelTimingMonitors;
	std::unique_ptr<TrackMonitor> m_pTrackMonitor;
	std::unique_ptr<PulseMonitor> m_pPulseMonitor;
	int m_iNumberOfPanelsWithPrimaryPulse;
};
