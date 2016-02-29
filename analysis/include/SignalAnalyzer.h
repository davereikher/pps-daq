#pragma once
#include <vector>
#include <cfloat>
#include <limits.h>

#define NO_PULSE_EDGE INT_MAX
#define NO_PULSE_MINIMUM_VALUE FLT_MAX


class SignalAnalyzer
{
public:
	struct AnalysisMarkers
	{
		int m_iPulseThreshold;
		int m_iEdgeThreshold;
		int m_iExpectedPulseWidth;
		int m_iMinEdgeSeparation;
		int m_iMaxEdgeJitter;
		int m_iMaxAmplitudeJitter;
		std::vector<std::pair<int, float> > m_vChannelsEdgeAndMinimum;
		std::vector<int> m_vChannelsWithPulse;
	};
public:
	SignalAnalyzer();
	std::pair<int, float> FindLeadingEdgeAndPulseHeight(std::vector<float>& a_samplesVector);
	void FindOriginalPulseInChannelRange(std::vector<std::vector<float> >& a_vAllChannels, std::vector<int>& a_vRange);
	bool DoesRangeHaveSignal(std::vector<std::vector<float> >& a_vAllChannels, std::vector<int> a_vRange);
	AnalysisMarkers& GetAnalysisMarkers();

private:
	AnalysisMarkers m_markers;
};
