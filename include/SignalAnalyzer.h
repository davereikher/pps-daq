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


#define NO_PULSE_EDGE INT_MAX
#define NO_PULSE_MINIMUM_VALUE FLT_MAX
#define NO_PULSE_MIN_VALUE_LOCATION INT_MAX

using namespace std::chrono;

class SignalAnalyzer
{
public:
	class AnalysisMarkers
	{
		class Value
		{
		public:
			Value(int a_iDiscrete, float a_fContinuous):
			m_iDiscrete(a_iDiscrete), m_fContinuous(a_fContinuous)
			{}
			int Discrete() {return m_iDiscrete;}
			float Continuous() {return m_fContinuous;}
		private:
			int m_iDiscrete;
			float m_fContinuous;
		};
	public:
		void ConfigureVoltageConversion(float a_fVoltMin, float a_fVoltMax, int a_iDigitizerResolution);
		void ConfigureTimeConversion(float a_fSamplingFreqGHz);
		void SetPulseThreshold(float a_fPulseThresholdVolts);
		void SetEdgeThreshold(float a_fEdgeThresholdVolts);
		void SetExpectedPulseWidth(float a_fExpectedPulseWidthNs);
		void SetMinEdgeSeparation(float a_fMinEdgeSeparationNs);
		void SetMaxEdgeJitter(float a_fMaxEdgeJitterNs);
		void SetMaxAmplitudeJitter(float a_fMaxAmplitudeJitterVolts);
		void SetPulseStartAmplitude(float a_fPulseStartThresholdVolts);
		void SetTriggerThreshold(float a_fTriggerThresholdVolts);
		void SetZero(float a_fVoltMin, float a_fVoltMax, int a_iDigitizerResolution);

		Value GetPulseThreshold();
		Value GetEdgeThreshold();
		Value GetExpectedPulseWidth();
		Value GetMinEdgeSeparation();
		Value GetMaxEdgeJitter();
		Value GetMaxAmplitudeJitter();
		Value GetPulseStartAmplitude();
		Value GetTriggerThreshold();
		Value GetZero();

	public:
		float m_fVoltageDivisionVolts;
		float m_fVoltageStartVolts;
		float m_fTimeDivisionNs;

		int m_iPulseThreshold;
		float m_fPulseThreshold;

		int m_iEdgeThreshold;
		float m_fEdgeThreshold;

		int m_iExpectedPulseWidth;
		float m_fExpectedPulseWidth;

		int m_iMinEdgeSeparation;
		float m_fMinEdgeSeparation;

		int m_iMaxEdgeJitter;
		float m_fMaxEdgeJitter;

		int m_iMaxAmplitudeJitter;
		float m_fMaxAmplitudeJitter;

		int m_iPulseStartAmplitude;
		float m_fPulseStartAmplitude;
		
		int m_iTriggerThreshold;
		float m_fTriggerThreshold;

		int m_iZero;
		
		std::vector<std::tuple<DataPoint, DataPoint> > m_vChannelsEdgeAndMinimum;
		std::vector<int> m_vChannelsWithPulse;
	};

	enum AnalysisFlags
	{
		EAsynchronous = 0x00000001,
		ETriggerTimingMonitor = 0x00000002,
		EPanelHitMonitor = 0x00000004,
		EPanelTimingMonitor = 0x00000008,
		ETrackMonitor = 0x00000010,
		ECountPanelsWithPrimaryPulse = 0x00000020,
		EPanelDegradationMonitor = 0x00000040,
		EPulseMonitor = 0x00000080
	};
public:
	SignalAnalyzer();
	std::tuple<DataPoint, DataPoint> FindLeadingEdgeAndPulseExtremum(std::vector<float>& a_samplesVector);
	void FindOriginalPulseInChannelRange(Channels_t& a_vAllChannels, std::string a_sPanelName, std::vector<int>& a_vRange);
	DataPoint FindTriggerTime(Channels_t& a_vAllChannels);
	void Analyze(nanoseconds a_eventTimeFromStart, Channels_t& a_vChannels);

	AnalysisMarkers& GetAnalysisMarkers();
	void Start();
	void Stop();
	void SetFlags(int a_iFlags);
	void Flush();
	float FindOffsetVoltage(std::vector<float> a_vSamples, int a_iChannelNum);
	Channels_t NormalizeChannels(Channels_t& a_vChannels);
	void ProcessEvents();
	void Configure(std::string a_sPanelName);
	int GetLastNumberOfPanelsWithPrimaryPulse();
	void AnalyzeTrack(HitMap_t& a_panelAndLine);

private:
	static void MainAnalysisThreadFunc(SignalAnalyzer* a_pSignalAnalyzer);
	void DoAnalysis(nanoseconds a_timeStamp, Channels_t& a_vChannels);

private:
	AnalysisMarkers m_markers;
	Range_t m_vRanges;
	float m_fTimeDivisionNs;
	float m_fVoltageDivisionVolts;
	float m_fVoltageStartVolts;
	int m_iFlags;
	bool m_bStopAnalysisThread;
	std::thread m_analysisThread;
	std::mutex m_mutex;
	Queue<std::pair<nanoseconds, Channels_t> > m_queue;
	std::unique_ptr<TriggerTimingMonitor> m_pTriggerTimingMonitor;
	std::vector<std::unique_ptr<PanelMonitor> > m_vpPanelMonitors;
	std::vector<std::unique_ptr<PanelDegradationMonitor> > m_vpPanelDegradationMonitors;
	std::vector<std::unique_ptr<PanelTimingMonitor> > m_vpPanelTimingMonitors;
	std::unique_ptr<TrackMonitor> m_pTrackMonitor;
	std::unique_ptr<PulseMonitor> m_pPulseMonitor;
	int m_iNumberOfPanelsWithPrimaryPulse;
};
