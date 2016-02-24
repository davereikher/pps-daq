#pragma once
#include <vector>
#include <cfloat>
#include <limits.h>

#define NO_PULSE_EDGE INT_MAX
#define NO_PULSE_MINIMUM_VALUE FLT_MAX


class SignalAnalyzer
{
public:

	static std::pair<int, float> FindLeadingEdgeAndPulseHeight(std::vector<float>& a_samplesVector);
	static std::vector<int> FindOriginalPulseInChannelRange(std::vector<std::vector<float> >& a_vAllChannels, std::vector<int>& a_vRange);
	static bool DoesRangeHaveSignal(std::vector<std::vector<float> >& a_vAllChannels, std::vector<int> a_vRange);
};
