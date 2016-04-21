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
	float GetPulseThresholdVolts(std::string a_sPanelName);
	float GetEdgeThresholdVolts(std::string a_sPanelName);
	float GetVoltMin();
	float GetVoltMax();
	int GetDigitizerResolution();
	float GetExpectedPulseWidthNs(std::string a_sPanelName);
	float GetMinEdgeSeparationNs(std::string a_sPanelName);
	float GetMaxEdgeJitterNs(std::string a_sPanelName);
	float GetMaxAmplitudeJitterVolts(std::string a_sPanelName);
	float GetSamplingFreqGHz();
	int GetNumberOfEventsToDrawAfter();
	seconds GetTriggerRateAveragingDurationSecs();
	float GetIdleFluctuationsAmplitude();
	float GetIdleLineDurationFraction();
	bool ShouldNormalizeChannels();
	float GetPulseStartAmplitudeVolts(std::string a_sPanelName);
	float GetTriggerThresholdVolts();
	int GetLineCorrespondingTo(std::string a_sPanel, int a_iChannel);
	int GetPanelIndex(std::string a_sPanelName);
	float GetDistanceBetweenPanelsMm();
	float GetReadoutLinePitchMm();
	int GetNumberOfEventsToFitAfter();
	bool ShouldFitTimingHistograms();
	float GetCrystalBallAlphaParam();
	float GetCrystalBallnParam();
	float GetCrystalBallMuParam();
	float GetCrystalBallSigmaParam();
	float GetCrystalBallNParam();

	std::string GetDump();

private:
	bool m_bInitialized;
	Json::Value m_configuration;
	static Configuration m_instance;
};
