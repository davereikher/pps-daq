#include <fstream>
#include <sstream>
#include <iostream>
#include "Configuration.h"

Configuration Configuration::m_instance;

Configuration::Configuration():
m_bInitialized(false)
{
}

Configuration& Configuration::Instance()
{
	return m_instance;
}

void Configuration::LoadConfiguration(const char* a_pFilename) {
	//TODO: check exceptions
	printf ("File name: %s\n", a_pFilename);
	std::ifstream in(a_pFilename, std::ifstream::binary);
	std::cout << "BEFORE" << std::endl;
	Json::Reader reader;
	int successful = reader.parse(in, m_configuration, false );

	if(!successful)
	{
		//TODO: exception here
		std::cout << "Could not read json. Error: " << reader.getFormattedErrorMessages() << std::endl;
	}

	std::cout << "OK" <<std::endl;
	in.close();
	m_bInitialized = true;

	Json::Value ranges = m_configuration["panels"];
	Json::Value::Members panelKeys = ranges.getMemberNames();

	for (auto& panelKey: panelKeys) 
	{
		m_mIndexToPanel[ranges[panelKey]["index"].asInt()] = panelKey;
		std::vector<int> singlePanelRange;
		//printf("\npanel key = %s\n", panelKey.c_str());
		for (auto& channelIt: ranges[panelKey]["channels"])
		{
			//printf("%d ", channelIt.asInt());
			singlePanelRange.push_back(channelIt.asInt());
		}
		m_vRanges[panelKey]= singlePanelRange;
	}
}


std::map<std::string, std::vector<int> > Configuration::GetRanges()
{
		return m_vRanges;
}

int Configuration::GetNumberOfPanels()
{
	return m_vRanges.size();
}

float Configuration::GetPulseThresholdVolts(std::string a_sPanelName)
{
	return m_configuration["panels"][a_sPanelName]["pulse-threshold-volts"].asFloat();
}

float Configuration::GetEdgeThresholdVolts(std::string a_sPanelName)
{
	//printf("\nEDGE THRESHOLD VOLTS FOR %s is %f\n", a_sPanelName.c_str(), m_configuration["panels"][a_sPanelName]["edge-threshold-volts"].asFloat());
	return m_configuration["panels"][a_sPanelName]["edge-threshold-volts"].asFloat();
}

float Configuration::GetVoltMin()
{
	return m_configuration["volt-min"].asFloat();
}

float Configuration::GetVoltMax()
{
	return m_configuration["volt-max"].asFloat();
}

int Configuration::GetDigitizerResolution()
{
	return m_configuration["digitizer-resolution-bits"].asInt();
}

float Configuration::GetExpectedPulseWidthNs(std::string a_sPanelName)
{
	return m_configuration["panels"][a_sPanelName]["expected-pulse-width-ns"].asFloat();
}

float Configuration::GetMinEdgeSeparationNs(std::string a_sPanelName)
{
	return m_configuration["panels"][a_sPanelName]["min-edge-separation-ns"].asFloat();
}

float Configuration::GetMaxEdgeJitterNs(std::string a_sPanelName)
{
	return m_configuration["panels"][a_sPanelName]["max-edge-jitter-ns"].asFloat();
}

float Configuration::GetMaxAmplitudeJitterVolts(std::string a_sPanelName)
{
	return m_configuration["panels"][a_sPanelName]["max-amplitude-jitter-volts"].asFloat();
}

float Configuration::GetSamplingFreqGHz()
{
	return m_configuration["sampling-freq-ghz"].asFloat();
}

int Configuration::GetNumberOfEventsToDrawAfter()
{
	return m_configuration["number-of-events-to-draw-after"].asInt();
}

seconds Configuration::GetTriggerRateAveragingDurationSecs()
{
	return seconds(m_configuration["trigger-rate-averaging-duration-seconds"].asInt());
}

seconds Configuration::GetPanelDegradationAveragingDurationSecs()
{
	return seconds(m_configuration["panel-degradation-averaging-duration-seconds"].asInt());
}

float Configuration::GetIdleFluctuationsAmplitude()
{
	return m_configuration["idle-fluctuation-amplitude-volts"].asFloat();
}

float Configuration::GetIdleLineDurationFraction()
{
	return m_configuration["idle-line-duration-fraction"].asFloat();
}

std::string Configuration::GetDump()
{
	std::stringstream stream;
	stream << m_configuration;
	return std::string("\nConfiguration Dump \n-------------------------------------------\n") + stream.str() + "\n\n";
}

bool Configuration::ShouldNormalizeChannels()
{
	return m_configuration["normalize-channels"].asBool();
}
 
float Configuration::GetPulseStartAmplitudeVolts(std::string a_sPanelName)
{
	return m_configuration["panels"][a_sPanelName]["pulse-start-amplitude-volts"].asFloat();
}

float Configuration::GetTriggerThresholdVolts()
{
	return m_configuration["trigger-threshold-volts"].asFloat();
}

int Configuration::GetLineCorrespondingTo(std::string a_sPanel, int a_iChannel)
{	
//	printf("Getting line corresponding to %s, %d... ", a_sPanel.c_str(), a_iChannel);
	Json::Value ranges = m_configuration["panels"];
	int i = 0;

	for (auto& channelIt: ranges[a_sPanel]["channels"])
	{
		//printf("%d ", channelIt.asInt());
		if(channelIt.asInt() == a_iChannel)
		{
			return ranges[a_sPanel]["lines"][i].asInt();
		}	
		i ++;
	}
	return 0;
}

int Configuration::GetPanelIndex(std::string a_sPanelName)
{
	return m_configuration["panels"][a_sPanelName]["index"].asInt();
}

float Configuration::GetDistanceBetweenPanelsMm()
{
	return m_configuration["distance-between-panels-mm"].asFloat();
}

float Configuration::GetReadoutLinePitchMm()
{
	return m_configuration["readout-line-pitch-mm"].asFloat();
}

int Configuration::GetNumberOfEventsToFitAfter()
{
	return m_configuration["number-of-events-to-fit-after"].asInt();
}

bool Configuration::ShouldFitTimingHistograms()
{
	return m_configuration["should-fit-timing-histograms"].asBool();
}

float Configuration::GetCrystalBallAlphaParam()
{
	return m_configuration["crystal-ball-alpha"].asFloat();
}


float Configuration::GetCrystalBallnParam()
{	
	return m_configuration["crystal-ball-n"].asFloat();
}

float Configuration::GetCrystalBallMuParam()
{	
	return m_configuration["crystal-ball-mu"].asFloat();
}

float Configuration::GetCrystalBallSigmaParam()
{	
	return m_configuration["crystal-ball-sigma"].asFloat();
}
 
float Configuration::GetCrystalBallNParam()
{	
	return m_configuration["crystal-ball-N"].asFloat();
}

std::string Configuration::GetPanelNameWithIndex(int a_iIndex)
{
	return m_mIndexToPanel[a_iIndex];
}

float Configuration::GetMonteCarloPointDomainRectangleLengthXMm()
{
	return m_configuration["monte-carlo"]["track-intersection-point-domain-rectangle"]["length-x-mm"].asFloat();
}

float Configuration::GetMonteCarloPointDomainRectangleLengthYMm()
{
	return m_configuration["monte-carlo"]["track-intersection-point-domain-rectangle"]["length-y-mm"].asFloat();
}

float Configuration::GetMonteCarloPointDomainRectangleCenterXMm()
{
	return m_configuration["monte-carlo"]["track-intersection-point-domain-rectangle"]["center-x-mm"].asFloat();
}

float Configuration::GetMonteCarloPointDomainRectangleCenterYMm()
{
	return m_configuration["monte-carlo"]["track-intersection-point-domain-rectangle"]["center-y-mm"].asFloat();
}

float Configuration::GetMonteCarloPointDomainRectangleCenterZMm()
{	
	return m_configuration["monte-carlo"]["track-intersection-point-domain-rectangle"]["center-z-mm"].asFloat();
}

float Configuration::GetTopScintillatorLengthXMm()
{
	return m_configuration["monte-carlo"]["scintillators"]["top"]["length-x-mm"].asFloat();	
}

float Configuration::GetTopScintillatorLengthYMm()
{
	return m_configuration["monte-carlo"]["scintillators"]["top"]["length-y-mm"].asFloat();	
}

float Configuration::GetTopScintillatorCenterXMm()
{
	return m_configuration["monte-carlo"]["scintillators"]["top"]["center-x-mm"].asFloat();	
}

float Configuration::GetTopScintillatorCenterYMm()
{
	return m_configuration["monte-carlo"]["scintillators"]["top"]["center-y-mm"].asFloat();	
}

float Configuration::GetTopScintillatorCenterZMm()
{
	return m_configuration["monte-carlo"]["scintillators"]["top"]["center-z-mm"].asFloat();	
}

float Configuration::GetTopScintillatorEfficiency()
{
	return m_configuration["monte-carlo"]["scintillators"]["top"]["efficiency"].asFloat();	
}

float Configuration::GetBottomScintillatorLengthXMm()
{
	return m_configuration["monte-carlo"]["scintillators"]["bottom"]["length-x-mm"].asFloat();	
}

float Configuration::GetBottomScintillatorLengthYMm()
{
	return m_configuration["monte-carlo"]["scintillators"]["bottom"]["length-y-mm"].asFloat();	
}

float Configuration::GetBottomScintillatorCenterXMm()
{
	return m_configuration["monte-carlo"]["scintillators"]["bottom"]["center-x-mm"].asFloat();	
}

float Configuration::GetBottomScintillatorCenterYMm()
{
	return m_configuration["monte-carlo"]["scintillators"]["bottom"]["center-y-mm"].asFloat();	
}

float Configuration::GetBottomScintillatorCenterZMm()
{
	return m_configuration["monte-carlo"]["scintillators"]["bottom"]["center-z-mm"].asFloat();	
}

float Configuration::GetBottomScintillatorEfficiency()
{
	return m_configuration["monte-carlo"]["scintillators"]["bottom"]["efficiency"].asFloat();	
}

float Configuration::GetCenterXOfPanel(int a_iPanelIndex)
{
	return m_configuration["monte-carlo"]["panels"][a_iPanelIndex]["center-x-mm"].asFloat(); 
}

float Configuration::GetCenterYOfPanel(int a_iPanelIndex)
{
	return m_configuration["monte-carlo"]["panels"][a_iPanelIndex]["center-y-mm"].asFloat(); 
}

float Configuration::GetCenterZOfPanel(int a_iPanelIndex)
{
	return m_configuration["monte-carlo"]["panels"][a_iPanelIndex]["center-z-mm"].asFloat(); 
}

float Configuration::GetCellEfficiencyOfPanel(int a_iPanelIndex)
{
	return m_configuration["monte-carlo"]["panels"][a_iPanelIndex]["efficiency"].asFloat();
}

float Configuration::GetXPitchOfPanel(int a_iPanelIndex)
{
	return m_configuration["monte-carlo"]["panels"][a_iPanelIndex]["x-pitch-mm"].asFloat(); 
}

float Configuration::GetYPitchOfPanel(int a_iPanelIndex)
{
	return m_configuration["monte-carlo"]["panels"][a_iPanelIndex]["y-pitch-mm"].asFloat(); 
}

float Configuration::GetCellXLengthOfPanel(int a_iPanelIndex)
{
	return m_configuration["monte-carlo"]["panels"][a_iPanelIndex]["cell-length-x-mm"].asFloat(); 
}

float Configuration::GetCellYLengthOfPanel(int a_iPanelIndex)
{
	return m_configuration["monte-carlo"]["panels"][a_iPanelIndex]["cell-length-y-mm"].asFloat(); 
}

int Configuration::GetNumberOfROLinesOfPanel(int a_iPanelIndex)
{
	return m_configuration["monte-carlo"]["panels"][a_iPanelIndex]["number-of-RO-y-lines"].asInt(); 
}

int Configuration::GetNumberOfHVLinesOfPanel(int a_iPanelIndex)
{
	return m_configuration["monte-carlo"]["panels"][a_iPanelIndex]["number-of-HV-x-lines"].asInt(); 
}

float Configuration::GetNumberIonPairsPerMmOfPanel(int a_iPanelIndex)
{
	return m_configuration["monte-carlo"]["panels"][a_iPanelIndex]["ion-pairs-per-mm"].asFloat(); 
}

float Configuration::GetGasGapThicknessMmOfPanel(int a_iPanelIndex)
{
	return m_configuration["monte-carlo"]["panels"][a_iPanelIndex]["gas-gap-thickness-mm"].asFloat(); 
}

float Configuration::GetBreakdownGenerationGaussianSigmaMmOfPanel(int a_iPanelIndex)
{
	return m_configuration["monte-carlo"]["panels"][a_iPanelIndex]["breakdown-generation-gaussian-sigma-mm"].asFloat(); 
}

int Configuration::GetNumberOfMonteCarloPanels()
{
	return m_configuration["monte-carlo"]["panels"].size();
}

std::string Configuration::GetPanelMonteCarloName(int a_iPanelIndex)
{
	return m_configuration["monte-carlo"]["panels"][a_iPanelIndex]["name"].asString();
}

float Configuration::GetGlobalRotationAxisTheta()
{
	return m_configuration["monte-carlo"]["tracks"]["global-rotation"]["axis"]["theta"].asFloat();
}

float Configuration::GetGlobalRotationAxisPhi()
{
	return m_configuration["monte-carlo"]["tracks"]["global-rotation"]["axis"]["phi"].asFloat();
}

float Configuration::GetGlobalRotationAngle()
{
	return m_configuration["monte-carlo"]["tracks"]["global-rotation"]["angle"].asFloat();
}

float Configuration::GetNumberOfMonteCarloRuns()
{
	return m_configuration["monte-carlo"]["runs"].asInt();
}

std::string Configuration::GetImageFolderPath()
{
	return m_configuration["image-folder-path"].asString();
}

bool Configuration::UseSeparateCanvasesForStep()
{
	return m_configuration["step"]["use-separate-canvases-for-step"].asBool();
}

bool Configuration::ShowTriggerInWaveformsStep()
{
	return m_configuration["step"]["show-trigger-in-waveforms-step"].asBool();
}

bool Configuration::ShowEdgeThresholdMarkerStep()
{
	return m_configuration["step"]["show-edge-threshold-marker"].asBool();
}

bool Configuration::ShowPulseThresholdMarkerStep()
{
	return m_configuration["step"]["show-pulse-threshold-marker"].asBool();
}

bool Configuration::TagPrimaryPulseStep()
{
	return m_configuration["step"]["tag-primary-pulse"].asBool();
}

int Configuration::GetRateTimer()
{
	return m_configuration["rate-timer"].asInt(); 
}

float Configuration::GetAfterPulse_timer()
{
	return m_configuration["AfterPulse_timer"].asInt(); 
}

int Configuration::GetApplied_Voltage()
{
	return m_configuration["Applied_Voltage"].asInt(); 
}

