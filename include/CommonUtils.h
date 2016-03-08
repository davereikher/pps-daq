#pragma once
#include <vector>

class CommonUtils
{
public:

	static std::vector<float>& GenerateTimeSequence(unsigned int a_iNumOfSamples, float a_fSamplingFrequencyGHz = 0);
};
