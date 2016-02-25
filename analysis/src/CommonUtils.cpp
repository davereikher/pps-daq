#include "CommonUtils.h"

/**
Creates a time sequence for the time axis, for plotting, based on the provided sampling frequency and number of samples

@param a_iNumOfSamples: number of samples
@param a_iSamplingFrequencyGHz sampling frequency in GHz. If it's 0, the result will be just a sequence (0 to a_iNumOfSamples). Default value is 0.
@return a vector of time values, in nanoseconds
*/
std::vector<float> CommonUtils::GenerateTimeSequence(unsigned int a_iNumOfSamples, float a_iSamplingFrequencyGHz)
{
	float fTimeStep = 1.0/a_iSamplingFrequencyGHz;
	if (0 == a_iSamplingFrequencyGHz)
	{
		fTimeStep = 1;
	}
	std::vector<float> vResult;
	vResult.resize(a_iNumOfSamples);
	for (int i = 0; i < (int)a_iNumOfSamples; i++)
	{
		//vResult[i] = i * 0.4;
		vResult[i] = i * fTimeStep;
	}
	return vResult;
}


