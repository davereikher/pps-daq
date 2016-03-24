#include <chrono>
#include <unistd.h>
#include "TSystem.h"
#include "SignalAnalyzer.h"
#include "TApplication.h"
#include "Configuration.h"
#include "Logger.h"
#include "keyb.h"

std::vector <std::vector<int> > gvPanelChannelRanges;
static SignalAnalyzer::AnalysisMarkers m_markers;

//using std;
using namespace std::chrono;

/**
Constructor.
//TODO: move this comment somewhere else. It does not describe the params of the function anymore...
@param a_fSamplingFreqGHz - sampling frequency in GHz.
@param a_fVoltageMin - lowest voltage in the dynamic range of the digitizer.
@param a_fVoltageMax - highest voltage in the dynamic range of the digitizer.
@param a_iDigitizerResolution - resolution of the digitizer (i.e. 0x00000fff for 12 bit).
@param a_fPulseThresholdVolts - if a sample in a channel is found below it, this channel will be considered to have a pulse.
@param a_fEdgeThresholdVolts - the time at which the signal in a channel first reaches it will be the time of the leading edge of the first pulse in that channel.
@param a_fExpectedPulseWidthNs - the size of the window after the leading edge of a pulse to look for the minimum value of the signal in (the sample with the lowest numeric value).
@param a_fMinEdgeSeparationNs - the minimum amount of time the first detected pulse in a range of channels arrives before the following one in that same range for that pulse to be considered as the original one.
@param a_fMaxEdgeJitterNs - the length of the window from the leading edge of the first pulse inside which pulses are considered to be "inseperable", that is, we cannot say just by the leading edge position which pulse comes first.
@param a_fMaxAmplitudeJitterVolts - the distance (in pulse amplitude units) from the lowest value of the largest pulse within which another pulse, if found, is not weak enough to be dismissed as a signal, and therefore all signals the lowest points of which fall within this window will be considered as potential pulses.
@param a_iDrawAftrNumOfevents - If any flags are set that tell SignalAnalyzer to plot anything, this is the number of events after which the canvases will be updated. Canvases should not be updated after every event because it's very slow.
*/
SignalAnalyzer::SignalAnalyzer():
m_iFlags(0),
m_bStopAnalysisThread(false),
m_pTriggerTimingSupervisor(new TriggerTimingSupervisor(milliseconds(Configuration::Instance().GetTriggerRateAveragingDurationSecs())))
{
	m_fVoltageStartVolts = Configuration::Instance().GetVoltMin();
	m_fVoltageDivisionVolts = (Configuration::Instance().GetVoltMax() - m_fVoltageStartVolts)/(float)Configuration::Instance().GetDigitizerResolution();
	m_fTimeDivisionNs = 1.0/Configuration::Instance().GetSamplingFreqGHz();

	m_markers.ConfigureVoltageConversion(m_fVoltageStartVolts, Configuration::Instance().GetVoltMax(), Configuration::Instance().GetDigitizerResolution());
	m_markers.ConfigureTimeConversion(Configuration::Instance().GetSamplingFreqGHz());
	
	m_markers.SetPulseThreshold(Configuration::Instance().GetPulseThresholdVolts());
	m_markers.SetEdgeThreshold(Configuration::Instance().GetEdgeThresholdVolts());
	m_markers.SetExpectedPulseWidth(Configuration::Instance().GetExpectedPulseWidthNs());
	m_markers.SetMinEdgeSeparation(Configuration::Instance().GetMinEdgeSeparationNs());
	m_markers.SetMaxEdgeJitter(Configuration::Instance().GetMaxEdgeJitterNs());
	m_markers.SetMaxAmplitudeJitter(Configuration::Instance().GetMaxAmplitudeJitterVolts());
	m_markers.SetPulseStartThreshold(Configuration::Instance().GetPulseStartThresholdVolts());

	for (auto it : Configuration::Instance().GetRanges())
	{
		printf("Pushing P.S. for %s\n", it.first.c_str());
		m_vpPanelSupervisors.push_back(std::unique_ptr<PanelSupervisor>(new PanelSupervisor(it.first)));
	}

	printf("PULSE_THRESHOLD: %f, %d\n", m_markers.GetPulseThreshold().Continuous(), m_markers.GetPulseThreshold().Discrete());
	printf("EDGE_THRESHOLD: %f, %d\n", m_markers.GetEdgeThreshold().Continuous(), m_markers.GetEdgeThreshold().Discrete());
	printf("EXPECTED_PULSE_WIDTH: %f, %d\n", m_markers.GetExpectedPulseWidth().Continuous(), m_markers.GetExpectedPulseWidth().Discrete());
	printf("MIN_EDGE_SEPARATION: %f, %d\n", m_markers.GetMinEdgeSeparation().Continuous(), m_markers.GetMinEdgeSeparation().Discrete());
	printf("MAX_EDGE_JITTER: %f, %d\n", m_markers.GetMaxEdgeJitter().Continuous(), m_markers.GetMaxEdgeJitter().Discrete());
	printf("MAX_AMPLITUDE_JITTER: %f, %d\n", m_markers.GetMaxAmplitudeJitter().Continuous(), m_markers.GetMaxAmplitudeJitter().Discrete());
}

void SignalAnalyzer::Start()
{
	if(m_iFlags & EAsynchronous)
	{
		m_analysisThread = std::thread(std::bind(&MainAnalysisThreadFunc, this));
	}
	else
	{
		TApplication* pApplication = new TApplication("app",0, 0);
	}	
}

/**
Maps a vector of samples to a tuple of values - the location of the leading edge of the first occurence of a pulse and the location of the lowest point on the pulse.
@param a_samplesVector - a vector of samples from a channel.
@return A tuple, where the first item is the location of the leading edge and the second is the location of the lowest point on the pulse.
*/
std::tuple<SignalAnalyzer::Point, SignalAnalyzer::Point> SignalAnalyzer::FindLeadingEdgeAndPulseExtremum(std::vector<float>& a_samplesVector)
{
	float minValue = FLT_MAX; 
	int minValueIndex = -1;
	int leadingEdge = -1;
	int leadingEdgeIndex = -1;
	int i = -1;	
	bool bChannelHasPulse = false;
	int iFluctuationStart = -1;

	for (auto& it: a_samplesVector)
	{
		i++;
		if (it < m_markers.GetPulseStartThreshold().Discrete())
		{
			iFluctuationStart = i;
		}

		if (iFluctuationStart != -1)
		{
			if (it < m_markers.GetPulseThreshold().Discrete())
			{
				bChannelHasPulse = true;
				break;
			}
	
			if ( (i - iFluctuationStart) > m_markers.GetExpectedPulseWidth().Discrete())
			{
				break;
			}
		}
	}

	if (!bChannelHasPulse)
	{
		return std::make_tuple(SignalAnalyzer::Point(0, 0, 0, 0, 0, false), SignalAnalyzer::Point(0, 0, 0, 0, 0, false));
	}

	for (i = 0; i < (int)a_samplesVector.size(); i++)
	{
		if (a_samplesVector[i] < m_markers.GetEdgeThreshold().Discrete())
		{
		//Reached a leading edge. This is a pulse on this channel (not necessarily the original pulse). 
		//Look inside a window EXPECTED_PULSE_WIDTH wide for the lowest value
			leadingEdge = i;
			break;
		}
	}

	if (i == (int)a_samplesVector.size())
	{
		//no leading edge found => no pulse
		return std::make_tuple(SignalAnalyzer::Point(0, 0, 0, 0, 0, false), SignalAnalyzer::Point(0, 0, 0, 0, 0, false));
	}

	leadingEdgeIndex = i;

	int window_end = i + m_markers.GetExpectedPulseWidth().Discrete(); 

	for ( ; i < window_end; i++)
	{
		if (a_samplesVector[i] < minValue)
		{
			minValue = a_samplesVector[i];
			minValueIndex = i;
		}
	}
	

	return std::make_tuple(SignalAnalyzer::Point(leadingEdgeIndex, m_markers.GetEdgeThreshold().Discrete(), m_fVoltageStartVolts, m_fTimeDivisionNs, m_fVoltageDivisionVolts), 
			SignalAnalyzer::Point(minValueIndex, minValue, m_fVoltageStartVolts, m_fTimeDivisionNs, m_fVoltageDivisionVolts));
}

/**
Analyzes a range of the channels to find the original pulse.
- For each event:
	- For each range:
		- Map the vectors of samples associated with each channel in that range to a vector of tuples (leading_edge, min_pulse_value)
		- In the vector of tuples find the lowest and the next to lowest value of leading_edge. 
		- If these values are farther away than MIN_EDGE_SEPARATION:
			- Take the one with the lowest value as the signal.
		- Otherwise:
			- Take all channels the leading_edge on which are bundled within a distance MAX_EDGE_JITTER from the first leading_edge and find the channel with the lowest min_pulse_value.
			- Return all channel numbers the min_pulse_value of which are bundled withing a window of size MAX_AMPLITUDE_JITTER upwards from the lowest min_pulse_value.
The result, a vector of channels containing the original pulse (ideally there will be just one item), is placed in m_markers.m_vChannelsWithPulse. The tuples returned by FindLeadingEdgeAndPulseExtremum are also stored in m_markers.m_vChannelsEdgeAndMinimum.

@param a_vAllChannels - a vector of all 32 channels, where each item is a vector of samples from that channel.
@param a_vRange - a vector of indices of the interesting channels in the first parameter.
*/
void SignalAnalyzer::FindOriginalPulseInChannelRange(Channels_t& a_vAllChannels,  std::vector<int>& a_vRange)
{

	//Get all channels in the provided range, find the leading edge and minimum value of the pulses and store the result in a vector of tuples. Also, find the earliest and next-to-earliest leading edge in the channels in the range.
	int iEarliestLeadingEdge = INT_MAX;
	int iNextToEarliestLeadingEdge = INT_MAX;
	int iEarliestLeadingEdgeIndex = -1;
	m_markers.m_vChannelsWithPulse.clear();
	m_markers.m_vChannelsEdgeAndMinimum.clear();
	for (int i = 0; i < (int)a_vRange.size(); i++)
	{

	//Each tuple represents a pulse. The first value is the location of the leading edge time, the second is the location of the lowest value of the pulse
		std::tuple<SignalAnalyzer::Point, SignalAnalyzer::Point> p;
		p = FindLeadingEdgeAndPulseExtremum(a_vAllChannels[a_vRange[i]]);
		//printf("On channel %d\n", a_vRange[i]);
		m_markers.m_vChannelsEdgeAndMinimum.push_back(p);
		if(!std::get<EDGE_THRES_INDEX>(p).Exists())
		{
			continue;
		}
		//printf("\tLeading Edge at %f, %f, Pulse Extremum at %f, %f\n", std::get<EDGE_THRES_INDEX>(p).GetX(), std::get<EDGE_THRES_INDEX>(p).GetY(), std::get<MIN_PULSE_INDEX>(p).GetX(), std::get<MIN_PULSE_INDEX>(p).GetY());
		if (std::get<EDGE_THRES_INDEX>(p).GetXDiscrete() <= iEarliestLeadingEdge)
		{
			iNextToEarliestLeadingEdge = iEarliestLeadingEdge;
			iEarliestLeadingEdge = std::get<EDGE_THRES_INDEX>(p).GetXDiscrete();
			iEarliestLeadingEdgeIndex = i;
		}
		else if (std::get<EDGE_THRES_INDEX>(p).GetXDiscrete() < iNextToEarliestLeadingEdge)
		{
			iNextToEarliestLeadingEdge = std::get<EDGE_THRES_INDEX>(p).GetXDiscrete();
		}
	}
	//The leading pulse is separated by enough time from the following pulses so that it can be considered as the original pulse.
	if ((iNextToEarliestLeadingEdge - iEarliestLeadingEdge) >= m_markers.GetMinEdgeSeparation().Discrete())
	{
		m_markers.m_vChannelsWithPulse.push_back(a_vRange[iEarliestLeadingEdgeIndex]);
//		printf("LEADING PULSE FOUND\n");
		Logger().Instance().SetWriteCurrentMessage();
		Logger::Instance().AddMessage(std::string("Leading pulse, channel ") + std::to_string(a_vRange[iEarliestLeadingEdgeIndex]));
		//printf ("\tLeading pulse, channel %d\n", a_vRange[iEarliestLeadingEdgeIndex]);
		return;
	}
	else if (iEarliestLeadingEdge == INT_MAX)
	{
		//pulse not found on any channel
		return;
	}


	float minPulseValue = FLT_MAX;
	//Leading edges of first two signals are too close to determine which one came first
	//Analyze the amplitude of the bunched pulses to find the pulse with the largest one. That is (most likely) the original pulse.
	//However, if some of the bunched pulse's minimum values are too close to the lowest minimum value, we cannot definitely distinguish the original pulse. Therefore, we return all of them.
	//Find all pulses the leading edges of which are within a window of size MAX_EDGE_JITTER and find the pulse with the largest amplitude.
	std::vector<int> vChannelsWithBunchedPulses;
	for (int i = 0; i < (int)m_markers.m_vChannelsEdgeAndMinimum.size(); i++)
	{
		if(!(std::get<EDGE_THRES_INDEX>(m_markers.m_vChannelsEdgeAndMinimum[i]).Exists()))
		{
			continue;
		}

		if ((std::get<EDGE_THRES_INDEX>(m_markers.m_vChannelsEdgeAndMinimum[i]).GetXDiscrete() - iEarliestLeadingEdge) < m_markers.GetMaxEdgeJitter().Discrete())
		{
			if (std::get<MIN_PULSE_INDEX>(m_markers.m_vChannelsEdgeAndMinimum[i]).GetYDiscrete() < minPulseValue)
			{
				minPulseValue = std::get<MIN_PULSE_INDEX>(m_markers.m_vChannelsEdgeAndMinimum[i]).GetYDiscrete();
			}
			vChannelsWithBunchedPulses.push_back(i);
		}
	}
	
	//look for bunched minima of the pulse within the window MAX_AMPLITUDE_JITTER upwards from the lowest minimum:
	for (auto& iChannelNum: vChannelsWithBunchedPulses)
	{
		if ((std::get<MIN_PULSE_INDEX>(m_markers.m_vChannelsEdgeAndMinimum[iChannelNum]).GetYDiscrete() - minPulseValue) <= m_markers.GetMaxAmplitudeJitter().Discrete())
		{
			m_markers.m_vChannelsWithPulse.push_back(a_vRange[iChannelNum]);
		}	
	}
}

void SignalAnalyzer::Flush()
{
	if(m_iFlags & ETriggerTimingSupervisor)
	{
		m_pTriggerTimingSupervisor->Flush();
	}
}

/*
The purpose of this function is to compensate for small, constant (between event to event) DC offsets in the channels. These offsets can come, for example, from small differences in the resistances inside the attenuators used to connect cables to the digitizer. After normalization, all lines should have 0 DC offset.
*/
Channels_t SignalAnalyzer::NormalizeChannels(Channels_t& a_vChannels)
{

	Channels_t vNormalizedChannels;
	if ( !Configuration::Instance().ShouldNormalizeChannels() )
	{
		vNormalizedChannels = a_vChannels;
		return vNormalizedChannels;
	}

	int i = 0;
	float fVoltMin = Configuration::Instance().GetVoltMin();
	float fVoltMax = Configuration::Instance().GetVoltMax();
	float iDigitizerRes = (float)Configuration::Instance().GetDigitizerResolution();
	float fZeroLocation = iDigitizerRes * ((-1) * fVoltMin / (fVoltMax - fVoltMin) );
	for (auto chan: a_vChannels)
	{
		float fCorrection = fZeroLocation - FindOffsetVoltage(chan, i);
		std::vector<float> vChannel;
		for (auto sample: chan)
		{
			vChannel.push_back(sample + fCorrection);
		}
		vNormalizedChannels.push_back(vChannel);
		i++;
	}

	return vNormalizedChannels;
}

/*
Find the zero voltage of a channel in an event by looking for a straight line - one that doesn't change by more than idle-fluctuations-amplitude-volts configuration field, of length idle-line-duration-fraction of the total length of the acquisition window.
Operation principle:
	- Divide the samples vector into sections of samples of length idle-line-duration-fraction of the entire vector length, suppose there are N such sections
	- For each section in the N sections, calculate the highest and lowest values in this section.
		- If the difference between those values (the amplitude) is smaller than idle-fluctuation-amplitude-volts, return the average value of samples in that section	

@param a_vSamples - a vector of all samples from a single channel
@return the discretized (that is, in the raw format, before the conversion to volts) absolute value of the dc voltage for this channel
*/
float SignalAnalyzer::FindOffsetVoltage(std::vector<float> a_vSamples, int a_iChannelNum)
{
	int iStartIndex = 0;
	int iEndIndex = 0;
	
	int iNumberOfSamplesInSection = (int) (a_vSamples.size() * Configuration::Instance().GetIdleLineDurationFraction());
	float fMaximumAmplitude = Configuration::Instance().GetIdleFluctuationsAmplitude();
	bool bDone = false;
	while (!bDone)
	{
		iEndIndex = iStartIndex + iNumberOfSamplesInSection - 1;
		if ( iEndIndex >= (int)a_vSamples.size() - 1)
		{
			bDone = true;
			iEndIndex = (int)a_vSamples.size() - 1;
		}

		float fMaxValueInSection = FLT_MIN;
		float fMinValueInSection = FLT_MAX;
		float fAccumulator = 0;
		int i = iStartIndex;

		for (; i <= iEndIndex; i++)
		{
			fAccumulator += a_vSamples[i];

			if (a_vSamples[i] > fMaxValueInSection)
			{
				fMaxValueInSection = a_vSamples[i];
			}
			if (a_vSamples[i] < fMinValueInSection)
			{
				fMinValueInSection = a_vSamples[i];
			}
			if( fMaximumAmplitude - fMinValueInSection > fMaximumAmplitude)
			{
				break;
			}
		}

		if (i > iEndIndex)
		{
		//stable section found, returning the average value of samples on that section.
			return fAccumulator/(iEndIndex - iStartIndex);
		}

		iStartIndex = iEndIndex + 1;
	}

	//If no stable section found, return 0 and log.
	Logger::Instance().AddMessage(std::string("On channel ") + std::to_string(a_iChannelNum) + std::string(", stable section not found. Reference voltage is assumed 0."));
	Logger::Instance().SetWriteCurrentMessage();
	return 0;
}

/**
Returns whether a range has a pulse in one of the channels or not. This is good for panel efficiency measurements, for example.
@param a_vRange - the range of channels defining a panel
@return Whether a pulse was detected on one of the panels
*/
bool SignalAnalyzer::DoesRangeHaveSignal(Channels_t& a_vAllChannels, std::vector<int> a_vRange)
{
	FindOriginalPulseInChannelRange(a_vAllChannels, a_vRange);
	return !(m_markers.m_vChannelsWithPulse.empty());
}

SignalAnalyzer::AnalysisMarkers& SignalAnalyzer::GetAnalysisMarkers()
{
	return m_markers;
}

SignalAnalyzer::Point::Point()
{}

SignalAnalyzer::Point::Point(int a_iXDiscrete, int a_iYDiscrete, double a_fYStart, double a_fXConvertFactor, double a_fYConvertFactor, bool a_bExists):
m_fX(a_iXDiscrete * a_fXConvertFactor), m_fY(a_fYStart + a_iYDiscrete * a_fYConvertFactor), m_iXDiscrete(a_iXDiscrete), m_iYDiscrete(a_iYDiscrete), m_bExists(a_bExists)
{
	if(a_fYConvertFactor == 0)
	{
		m_fY = (float)a_iYDiscrete;
	}
	if(a_fXConvertFactor == 0)
	{
		m_fX = (float)a_iXDiscrete;
	}
	//printf("a_fYStart: %f, a_fYConvertFactor = %f\n", a_fYStart, a_fYConvertFactor);
}

double SignalAnalyzer::Point::GetX()
{
	return m_fX;
}

double SignalAnalyzer::Point::GetY()
{
	return m_fY;
}

bool SignalAnalyzer::Point::Exists()
{
	return m_bExists;
}

int SignalAnalyzer::Point::GetXDiscrete()
{
	return m_iXDiscrete;
}

int SignalAnalyzer::Point::GetYDiscrete()
{
	return m_iYDiscrete;
}

void SignalAnalyzer::AnalysisMarkers::ConfigureVoltageConversion(float a_fVoltMin, float a_fVoltMax, int a_iDigitizerResolution)
{
	m_fVoltageDivisionVolts = (a_fVoltMax - a_fVoltMin)/(float)a_iDigitizerResolution;
	m_fVoltageStartVolts = a_fVoltMin;
	printf("digires: %d, min volts: %f, max volts: %f, start volts: %f, voltageDiv: %f\n", a_iDigitizerResolution, a_fVoltMin, a_fVoltMax, m_fVoltageStartVolts, m_fVoltageDivisionVolts);
}

void SignalAnalyzer::AnalysisMarkers::ConfigureTimeConversion(float a_fSamplingFreqGHz)
{
	m_fTimeDivisionNs = 1.0/a_fSamplingFreqGHz;
}

void SignalAnalyzer::AnalysisMarkers::SetPulseThreshold(float a_fPulseThresholdVolts)
{
	m_iPulseThreshold = (a_fPulseThresholdVolts - m_fVoltageStartVolts) / m_fVoltageDivisionVolts;
	m_fPulseThreshold = a_fPulseThresholdVolts;
}

void SignalAnalyzer::AnalysisMarkers::SetPulseStartThreshold(float a_fPulseStartThresholdVolts)
{
	m_iPulseStartThreshold = (a_fPulseStartThresholdVolts - m_fVoltageStartVolts) / m_fVoltageDivisionVolts;
	m_fPulseStartThreshold = a_fPulseStartThresholdVolts;
}

void SignalAnalyzer::AnalysisMarkers::SetEdgeThreshold(float a_fEdgeThresholdVolts)
{
	m_iEdgeThreshold = (a_fEdgeThresholdVolts - m_fVoltageStartVolts) / m_fVoltageDivisionVolts;
	m_fEdgeThreshold = a_fEdgeThresholdVolts;
}

void SignalAnalyzer::AnalysisMarkers::SetExpectedPulseWidth(float a_fExpectedPulseWidthNs)
{
	m_iExpectedPulseWidth = a_fExpectedPulseWidthNs / m_fTimeDivisionNs;
	m_fExpectedPulseWidth = a_fExpectedPulseWidthNs;
}

void SignalAnalyzer::AnalysisMarkers::SetMinEdgeSeparation(float a_fMinEdgeSeparationNs)
{
	m_iMinEdgeSeparation = a_fMinEdgeSeparationNs / m_fTimeDivisionNs;
	m_fMinEdgeSeparation = a_fMinEdgeSeparationNs;
}


void SignalAnalyzer::AnalysisMarkers::SetMaxEdgeJitter(float a_fMaxEdgeJitterNs)
{
	m_iMaxEdgeJitter = a_fMaxEdgeJitterNs / m_fTimeDivisionNs;
	m_fMaxEdgeJitter = a_fMaxEdgeJitterNs;
}

void SignalAnalyzer::AnalysisMarkers::SetMaxAmplitudeJitter(float a_fMaxAmplitudeJitterVolts)
{
	m_iMaxAmplitudeJitter = a_fMaxAmplitudeJitterVolts / m_fVoltageDivisionVolts;
	m_fMaxAmplitudeJitter = a_fMaxAmplitudeJitterVolts;
}


SignalAnalyzer::AnalysisMarkers::Value SignalAnalyzer::AnalysisMarkers::GetPulseThreshold()
{
	return Value(m_iPulseThreshold, m_fPulseThreshold);
}

SignalAnalyzer::AnalysisMarkers::Value SignalAnalyzer::AnalysisMarkers::GetEdgeThreshold()
{
	return Value(m_iEdgeThreshold, m_fEdgeThreshold);
}

SignalAnalyzer::AnalysisMarkers::Value SignalAnalyzer::AnalysisMarkers::GetExpectedPulseWidth()
{
	return Value(m_iExpectedPulseWidth, m_fExpectedPulseWidth);
}

SignalAnalyzer::AnalysisMarkers::Value SignalAnalyzer::AnalysisMarkers::GetMinEdgeSeparation()
{
	return Value(m_iMinEdgeSeparation, m_fMinEdgeSeparation);
}

SignalAnalyzer::AnalysisMarkers::Value SignalAnalyzer::AnalysisMarkers::GetMaxEdgeJitter()
{
	return Value(m_iMaxEdgeJitter, m_fMaxEdgeJitter);
}

SignalAnalyzer::AnalysisMarkers::Value SignalAnalyzer::AnalysisMarkers::GetMaxAmplitudeJitter()
{
	return Value(m_iMaxAmplitudeJitter, m_fMaxAmplitudeJitter);
}

SignalAnalyzer::AnalysisMarkers::Value SignalAnalyzer::AnalysisMarkers::GetPulseStartThreshold()
{
	return Value(m_iPulseStartThreshold, m_fPulseStartThreshold);
}

void SignalAnalyzer::MainAnalysisThreadFunc(SignalAnalyzer* a_pSignalAnalyzer)
{
	static int iEventNum = 0;
	
	//The constructor of TApplication causes a segmentation violation, so we instantiate it on the heap and not delete it at the end. This is bad, but not fatal.
	TApplication* pApplication = new TApplication("app",0, 0);

	while(true)
	{
		auto pair = a_pSignalAnalyzer->m_queue.pop();
		{
			std::lock_guard<std::mutex> lockGuard(a_pSignalAnalyzer->m_mutex);
			//printf("stopAnalysisthread = %d\n", a_pSignalAnalyzer->m_bStopAnalysisThread);
			if (a_pSignalAnalyzer->m_bStopAnalysisThread)
			{
			//	printf("breaking\n");
				break;
			}
		}

//		printf("AFTER RELEASE\n");

		a_pSignalAnalyzer->DoAnalysis(pair.first, pair.second);

//		gSystem->ProcessEvents();
	}
}

void SignalAnalyzer::DoAnalysis(nanoseconds a_timeStamp, Channels_t& a_vChannels)
{
	Channels_t vNormalizedChannels = NormalizeChannels(a_vChannels);
	if (m_iFlags & AnalysisFlags::ETriggerTimingSupervisor)
	{
		m_pTriggerTimingSupervisor->GotTrigger(a_timeStamp);
	}
	if (m_iFlags & AnalysisFlags::EPanelSupervisor)
	{
		if(a_vChannels.size() == 1)
		{
			if(a_vChannels[0].size() == 1)
			{
				ProcessEvents();
				return;
			}
		}

		int i = 0;
		for (auto it: Configuration::Instance().GetRanges())
		{
			FindOriginalPulseInChannelRange(vNormalizedChannels, it.second);
			m_vpPanelSupervisors[i]->GotEvent(a_timeStamp, m_markers.m_vChannelsWithPulse);
			i++;
		}
		
	}

	ProcessEvents();
}

void SignalAnalyzer::ProcessEvents()
{
	gSystem->ProcessEvents();
}

void SignalAnalyzer::Stop()
{
	if(m_iFlags & EAsynchronous)
	{	//separate block for lock_guard
		{	
			std::lock_guard<std::mutex> lockGuard(m_mutex);
			//printf("Setting\n");
			m_bStopAnalysisThread = true;
			m_queue.unlock();
		}	
		//printf("joining\n");
		m_analysisThread.join();
		//printf("after joining\n");
	}
}

/**
Set which actions must be performed during analysis

@param: a_iFlags. Can take the following values:
	AVG_TRIG_RATE - every X seconds, plot the average trigger rate.
	AVG_PANEL_COINCIDENCE_RATES - every Y seconds, plot the average rate of coincidence for each panel with the trigger, on a split canvas.
	AVG_TRACK_DETECTION_RATE - every Z seconds, plot 
*/
void SignalAnalyzer::SetFlags(int a_iFlags)
{
	m_iFlags = a_iFlags;
}

void SignalAnalyzer::Analyze(nanoseconds a_eventTimeFromStart, Channels_t& a_vChannels)
{	

	if(m_iFlags & EAsynchronous)
	{
		m_queue.push(std::pair<nanoseconds, std::vector<std::vector<float> > >(a_eventTimeFromStart, a_vChannels));
	}
	else
	{
		//printf("synchronous\n");
		DoAnalysis(a_eventTimeFromStart, a_vChannels);
	}
}

