#include "SignalAnalyzer.h"
std::vector <std::vector<int> > gvPanelChannelRanges;
static SignalAnalyzer::AnalysisMarkers m_markers;

/**
Constructor.

@param a_fSamplingFreqGHz - sampling frequency in GHz.
@param a_fVoltageMin - lowest voltage in the dynamic range of the digitizer.
@param a_fVoltageMax - highest coltage in the dynamic range of the digitizer.
@param a_iDigitizerResolution - resolution of the digitizer (i.e. 0x00000fff for 12 bit).
@param a_fPulseThresholdVolts - if a sample in a channel is found below it, this channel will be considered to have a pulse.
@param a_fEdgeThresholdVolts - the time at which the signal in a channel first reaches it will be the time of the leading edge of the first pulse in that channel.
@param a_fExpectedPulseWidthNs - the size of the window after the leading edge of a pulse to look for the minimum value of the signal in (the sample with the lowest numeric value).
@param a_fMinEdgeSeparationNs - the minimum amount of time the first detected pulse in a range of channels arrives before the following one in that same range for that pulse to be considered as the original one.
@param a_fMaxEdgeJitterNs - the length of the window from the leading edge of the first pulse inside which pulses are considered to be "inseperable", that is, we cannot say just by the leading edge position which pulse comes first.
@param a_fMaxAmplitudeJitterVolts - the distance (in pulse amplitude units) from the lowest value of the largest pulse within which another pulse, if found, is not weak enough to be dismissed as a signal, and therefore all signals the lowest points of which fall within this window will be considered as potential pulses.
*/
SignalAnalyzer::SignalAnalyzer(float a_fSamplingFreqGHz, float a_fVoltageMin, float a_fVoltageMax, int a_iDigitizerResolution,
float a_fPulseThresholdVolts, float a_fEdgeThresholdVolts, float a_fExpectedPulseWidthNs, float a_fMinEdgeSeparationNs, float a_fMaxEdgeJitterNs, float a_fMaxAmplitudeJitterVolts)
{
	m_fVoltageStartVolts = a_fVoltageMin;
	m_fVoltageDivisionVolts = (a_fVoltageMax - a_fVoltageMin)/(float)a_iDigitizerResolution;
	m_fTimeDivisionNs = 1.0/a_fSamplingFreqGHz;

	m_markers.ConfigureVoltageConversion(a_fVoltageMin, a_fVoltageMax, a_iDigitizerResolution);
	m_markers.ConfigureTimeConversion(a_fSamplingFreqGHz);
	
	m_markers.SetPulseThreshold(a_fPulseThresholdVolts);
	m_markers.SetEdgeThreshold(a_fEdgeThresholdVolts);
	m_markers.SetExpectedPulseWidth(a_fExpectedPulseWidthNs);
	m_markers.SetMinEdgeSeparation(a_fMinEdgeSeparationNs);
	m_markers.SetMaxEdgeJitter(a_fMaxEdgeJitterNs);
	m_markers.SetMaxAmplitudeJitter(a_fMaxAmplitudeJitterVolts);

	printf("PULSE_THRESHOLD: %f, %d\n", m_markers.GetPulseThreshold().Continuous(), m_markers.GetPulseThreshold().Discrete());
	printf("EDGE_THRESHOLD: %f, %d\n", m_markers.GetEdgeThreshold().Continuous(), m_markers.GetEdgeThreshold().Discrete());
	printf("EXPECTED_PULSE_WIDTH: %f, %d\n", m_markers.GetExpectedPulseWidth().Continuous(), m_markers.GetExpectedPulseWidth().Discrete());
	printf("MIN_EDGE_SEPARATION: %f, %d\n", m_markers.GetMinEdgeSeparation().Continuous(), m_markers.GetMinEdgeSeparation().Discrete());
	printf("MAX_EDGE_JITTER: %f, %d\n", m_markers.GetMaxEdgeJitter().Continuous(), m_markers.GetMaxEdgeJitter().Discrete());
	printf("MAX_AMPLITUDE_JITTER: %f, %d\n", m_markers.GetMaxAmplitudeJitter().Continuous(), m_markers.GetMaxAmplitudeJitter().Discrete());

/*	if(a_fSamplingFreqGHz == 0)
	{
		m_fTimeDivision_ns = 1;
	}
	else
	{
		m_fTimeDivision_ns = 1.0/a_fSamplingFreqGHz;
	}
	m_fVoltageDivision_volts = (a_fVoltageMax - a_fVoltageMin)/float(0x00000FFF);
	m_fVoltageStart_volts = a_fVoltageMin;*/
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

	for (auto& it: a_samplesVector)
	{
		if (it < m_markers.GetPulseThreshold().Discrete())
		{
			bChannelHasPulse = true;
			break;
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
void SignalAnalyzer::FindOriginalPulseInChannelRange(std::vector<std::vector<float> >& a_vAllChannels, std::vector<int>& a_vRange)
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
		m_markers.m_vChannelsEdgeAndMinimum.push_back(p);
		printf("X=%d, ",	std::get<EDGE_THRES_INDEX>(p).GetXDiscrete());
		if(!std::get<EDGE_THRES_INDEX>(p).Exists())
		{
			continue;
		}
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
	printf("Earliest: %d, Next to earliest: %d\n" , iEarliestLeadingEdge, iNextToEarliestLeadingEdge);
	//The leading pulse is separated by enough time from the following pulses so that it can be considered as the original pulse.
	if ((iNextToEarliestLeadingEdge - iEarliestLeadingEdge) >= m_markers.GetMinEdgeSeparation().Discrete())
	{
		m_markers.m_vChannelsWithPulse.push_back(a_vRange[iEarliestLeadingEdgeIndex]);
		printf("LEADING PULSE FOUND\n");
		return;
	}
	else if (iEarliestLeadingEdge == INT_MAX)
	{
		//pulse not found on any channel
		return;
	}


	float minPulseValue = FLT_MAX;
	//Leading edges of first two signals are too close to determine which one came first
	//Analyse the amplitude of the bunched pulses to find the pulse with the largest one. That is (most likely) the original pulse.
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

/**
Returns whether a range has a pulse in one of the channels or not. This is good for panel efficiency measurements, for example.
@param a_vRange - the range of channels defining a panel
@return Whether a pulse was detected on one of the panels
*/
bool SignalAnalyzer::DoesRangeHaveSignal(std::vector<std::vector<float> >& a_vAllChannels, std::vector<int> a_vRange)
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

