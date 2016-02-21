#pragma once
#include <vector>

class SignalAnalyzer
{
public:

	static std::pair<int, float> FindLeadingEdgeAndPulseAmplitude(std::vector<float>& a_samplesVector);
	static std::vector<int> FindOriginalPulseInChannelRange(std::vector<std::vector<float> >& a_vAllChannels, std::vector<int>& a_vRange);
	static bool DoesRangeHaveSignal(std::vector<std::vector<float> >& a_vAllChannels, std::vector<int> a_vRange);
};
