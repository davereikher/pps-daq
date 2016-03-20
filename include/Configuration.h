#pragma once

#include <chrono>
#include "json/json.h"

using namespace std::chrono;

class Configuration
{
public:
	static Configuration& Instance();
	Configuration();
	void LoadConfiguration(const char* a_pFilename);
	std::map<std::string, std::vector<int> > GetRanges();
	float GetPulseThresholdVolts();
	float GetEdgeThresholdVolts();
	float GetVoltMin();
	float GetVoltMax();
	int GetDigitizerResolution();
	float GetExpectedPulseWidthNs();
	float GetMinEdgeSeparationNs();
	float GetMaxEdgeJitterNs();
	float GetMaxAmplitudeJitterVolts();
	float GetSamplingFreqGHz();
	int GetNumberOfEventsToDrawAfter();
	seconds GetTriggerRateAveragingDurationSecs();
	float GetIdleFluctuationsAmplitude();
	float GetIdleLineDurationFraction();
	bool ShouldNormalizeChannels();
	float GetPulseStartThresholdVolts();


	std::string GetDump();

private:
	bool m_bInitialized;
	Json::Value m_configuration;
	static Configuration m_instance;
};
