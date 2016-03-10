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
#include "PanelSupervisor.h"
#include "TriggerTimingSupervisor.h"
#include "PanelSupervisor.h"


#define NO_PULSE_EDGE INT_MAX
#define NO_PULSE_MINIMUM_VALUE FLT_MAX
#define NO_PULSE_MIN_VALUE_LOCATION INT_MAX
#define EDGE_THRES_INDEX 0
#define MIN_PULSE_INDEX 1

using namespace std::chrono;

class SignalAnalyzer
{
public:
	class Point
	{
	public:
		Point(int a_iXDiscrete, int a_iYDiscrete, double a_fYStart, double a_fXConvertFactor = 1, double a_fYConvertFactor = 1, bool a_bExists = true);
		Point();
		double GetX();
		double GetY();
		int GetXDiscrete();
		int GetYDiscrete();
		bool Exists();
	
	private:
		double m_fX;
		double m_fY;
		int m_iXDiscrete;
		int m_iYDiscrete;
		bool m_bExists;
	};

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

		Value GetPulseThreshold();
		Value GetEdgeThreshold();
		Value GetExpectedPulseWidth();
		Value GetMinEdgeSeparation();
		Value GetMaxEdgeJitter();
		Value GetMaxAmplitudeJitter();

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

		std::vector<std::tuple<Point, Point> > m_vChannelsEdgeAndMinimum;
		std::vector<int> m_vChannelsWithPulse;
	};

	enum AnalysisFlags
	{
		EAsynchronous = 1,
		ETriggerTimingSupervisor = 2,
		EPanelSupervisor = 4
	};
public:
	SignalAnalyzer();
	std::tuple<Point, Point> FindLeadingEdgeAndPulseExtremum(std::vector<float>& a_samplesVector);
	void FindOriginalPulseInChannelRange(Channels_t& a_vAllChannels, std::vector<int>& a_vRange);
	bool DoesRangeHaveSignal(Channels_t& a_vAllChannels, std::vector<int> a_vRange);
	void Analyze(nanoseconds a_eventTimeFromStart, Channels_t& a_vChannels);

	AnalysisMarkers& GetAnalysisMarkers();
	void Start();
	void Stop();
	void SetFlags(int a_iFlags);
	void Flush();

private:
	static void MainAnalysisThreadFunc(SignalAnalyzer* a_pSignalAnalyzer);
	void DoAnalysis(nanoseconds a_timeStamp, Channels_t& a_channels);

private:
	AnalysisMarkers m_markers;
	float m_fTimeDivisionNs;
	float m_fVoltageDivisionVolts;
	float m_fVoltageStartVolts;
	int m_iFlags;
	bool m_bStopAnalysisThread;
	std::thread m_analysisThread;
	std::mutex m_mutex;
	Queue<std::pair<nanoseconds, Channels_t> > m_queue;
	std::unique_ptr<TriggerTimingSupervisor> m_pTriggerTimingSupervisor;
	std::vector<std::unique_ptr<PanelSupervisor> > m_vpPanelSupervisors;
};
