#pragma once
#include <vector>

class CommonUtils
{
public:

	static std::vector<int> GenerateTimeSequence(uint32_t a_iNumOfSamples, float a_iSamplingFrequencyGHz = 0);
};
