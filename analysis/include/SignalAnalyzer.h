#pragma once
#include <vector>
#include <cfloat>
#include <limits.h>
#include <tuple>

#define NO_PULSE_EDGE INT_MAX
#define NO_PULSE_MINIMUM_VALUE FLT_MAX
#define NO_PULSE_MIN_VALUE_LOCATION INT_MAX


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

	struct AnalysisMarkers
	{
		int m_iPulseThreshold;
		int m_iEdgeThreshold;
		int m_iExpectedPulseWidth;
		int m_iMinEdgeSeparation;
		int m_iMaxEdgeJitter;
		int m_iMaxAmplitudeJitter;
		std::vector<std::tuple<Point, Point> > m_vChannelsEdgeAndMinimum;
		std::vector<int> m_vChannelsWithPulse;
	};
public:
	SignalAnalyzer(float a_fSamplingFreqGHz, float a_fVoltageMin, float a_fVoltageMax, int a_iDigitizerResolution,
		float a_fPulseThresholdVolts, float a_fEdgeThresholdVolts, float a_fExpectedPulsewidthNs, 
		float a_fMinEdgeSeparationNs, float a_fMaxEdgeJitterNs, float a_fMaxAmplitudeJitterVolts);
	std::tuple<Point, Point> FindLeadingEdgeAndPulseExtremum(std::vector<float>& a_samplesVector);
	void FindOriginalPulseInChannelRange(std::vector<std::vector<float> >& a_vAllChannels, std::vector<int>& a_vRange);
	bool DoesRangeHaveSignal(std::vector<std::vector<float> >& a_vAllChannels, std::vector<int> a_vRange);
	AnalysisMarkers& GetAnalysisMarkers();

private:
	AnalysisMarkers m_markers;
	float m_fTimeDivision_ns;
	float m_fVoltageDivision_volts;
	float m_fVoltageStart_volts;
};
