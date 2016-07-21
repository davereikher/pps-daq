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
	int GetNumberOfPanels();
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
	seconds GetPanelDegradationAveragingDurationSecs();
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
	std::string GetPanelNameWithIndex(int a_iIndex);
	float GetMonteCarloPointDomainRectangleLengthXMm();
	float GetMonteCarloPointDomainRectangleLengthYMm();
	float GetMonteCarloPointDomainRectangleCenterXMm();
	float GetMonteCarloPointDomainRectangleCenterYMm();
	float GetMonteCarloPointDomainRectangleCenterZMm();
	float GetTopScintillatorLengthXMm();
	float GetTopScintillatorLengthYMm();
	float GetTopScintillatorCenterXMm();
	float GetTopScintillatorCenterYMm();
	float GetTopScintillatorCenterZMm();
	float GetTopScintillatorEfficiency();
	float GetBottomScintillatorLengthXMm();
	float GetBottomScintillatorLengthYMm();
	float GetBottomScintillatorCenterXMm();
	float GetBottomScintillatorCenterYMm();
	float GetBottomScintillatorCenterZMm();
	float GetBottomScintillatorEfficiency();
	float GetCenterXOfPanel(int a_iPanelIndex);
	float GetCenterYOfPanel(int a_iPanelIndex);
	float GetCenterZOfPanel(int a_iPanelIndex);	
	float GetCellEfficiencyOfPanel(int a_iPanelIndex);
	float GetXPitchOfPanel(int a_iPanelIndex);
	float GetYPitchOfPanel(int a_iPanelIndex);
	float GetCellXLengthOfPanel(int a_iPanelIndex);
	float GetCellYLengthOfPanel(int a_iPanelIndex);
	int GetNumberOfROLinesOfPanel(int a_iPanelIndex);
	int GetNumberOfHVLinesOfPanel(int a_iPanelIndex);
	int GetNumberOfMonteCarloPanels();
	std::string GetPanelMonteCarloName(int a_iPanelIndex);
	float GetNumberIonPairsPerMmOfPanel(int a_iPanelIndex);
	float GetGasGapThicknessMmOfPanel(int a_iPanelIndex);
	float GetBreakdownGenerationGaussianSigmaMmOfPanel(int a_iPanelIndex);
	float GetGlobalRotationAxisTheta();
	float GetGlobalRotationAxisPhi();
	float GetGlobalRotationAngle();
	float GetNumberOfMonteCarloRuns();
	std::string GetImageFolderPath();
	bool UseSeparateCanvasesForStep();

	std::string GetDump();

private:
	bool m_bInitialized;
	Json::Value m_configuration;
	static Configuration m_instance;
	std::map<int, std::string> m_mIndexToPanel;
	std::map<std::string, std::vector<int> > m_vRanges;
};
