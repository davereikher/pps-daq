#pragma once

#include "json/json.h"

class Configuration
{
public:
	static void LoadConfiguration(const char* a_pFilename);
	static std::map<std::string, std::vector<int> > GetRanges();
	static float GetPulseThresholdVolts();
	static float GetEdgeThresholdVolts();
	static float GetVoltMin();
	static float GetVoltMax();
	static float GetDigitizerResolution();
	static float GetExpectedPulseWidthNs();
	static float GetMinEdgeSeparationNs();
	static float GetMaxEdgeJitterNs();
	static float GetMaxAmplitudeJitterVolts();
	static float GetSamplingFreqGHz();

private:
	static bool m_bInitialized;
	static Json::Value m_configuration;
};
