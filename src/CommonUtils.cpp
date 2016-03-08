#include "CommonUtils.h"

/**
Creates a time sequence for the time axis, for plotting, based on the provided sampling frequency and number of samples

@param a_iNumOfSamples: number of samples
@param a_iSamplingFrequencyGHz sampling frequency in GHz. If it's 0, the result will be just a sequence (0 to a_iNumOfSamples). Default value is 0.
@return a vector of time values, in nanoseconds
*/
std::vector<float>& CommonUtils::GenerateTimeSequence(unsigned int a_iNumOfSamples, float a_fSamplingFrequencyGHz)
{
	static std::vector<float> vTimeSeq;
	static int iLastNumOfsamples = -1;
	static float fLastSamplingFrequencyGHz = -1;

	if((a_iNumOfSamples == iLastNumOfsamples) && (a_fSamplingFrequencyGHz == fLastSamplingFrequencyGHz))
	{
		return vTimeSeq;
	}

	iLastNumOfsamples = a_iNumOfSamples;
	fLastSamplingFrequencyGHz = a_fSamplingFrequencyGHz;

	float fTimeStep = 1.0/a_fSamplingFrequencyGHz;
	
	vTimeSeq.resize(a_iNumOfSamples);
	for (int i = 0; i < (int)a_iNumOfSamples; i++)
	{
		vTimeSeq[i] = i * fTimeStep;
	}

	return vTimeSeq;
}

