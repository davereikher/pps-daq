#include "SignalAnalyzer.h"
std::vector <std::vector<int> > gvPanelChannelRanges;
static SignalAnalyzer::AnalysisMarkers m_markers;

/*
We loop over all events. For each event, and for each range of channels (each range corresponds to a panel), we look for the channel that was hit by the following procedure:
	- Define a threshold PULSE_THRESHOLD, so that if a sample in a channel is found below it, this channel will be considered to have a pulse.
	- Define a threshold EDGE_THRESHOLD, so that the time at which the signal in a channel first reaches it will be the time of the leading edge of the first pulse in that channel.
	- Define a threshold EXPECTED_PULSE_WIDTH, which is the size of the window after the leading edge of a pulse to look for the minimum value of the signal in (the sample with the lowest numeric value).
	- Define a threshold MIN_EDGE_SEPARATION, which is the minimum amount of time the first detected pulse in a range of channels arrives before the following one in that same range for that pulse to be considered as the original one.
	- Define a threshold MAX_EDGE_JITTER, which is the length of the window from the leading edge of the first pulse inside which pulses are considered to be "inseperable", that is, we cannot say just by the leading edge position which pulse comes first.
	- Define a threshold MAX_AMPLITUDE_JITTER, which is the distance (in pulse height units) from the lowest value of the largest pulse within which another pulse, if found, is not weak enough to be dismissed as a signal, and therefore all signals the lowest points of which fall within this window will be considered as potential pulses.
*/


//TODO: export these into configuration
/*#define PULSE_THRESHOLD 1700 //TODO: set these values
#define EDGE_THRESHOLD 1840
#define EXPECTED_PULSE_WIDTH 70
#define MIN_EDGE_SEPARATION 100
#define MAX_EDGE_JITTER 50
#define MAX_AMPLITUDE_JITTER 200
*/
SignalAnalyzer::SignalAnalyzer(float a_fSamplingFreqGHz, float a_fVoltageMin, float a_fVoltageMax, int a_iDigitizerResolution,
float a_fPulseThresholdVolts, float a_fEdgeThresholdVolts, float a_fExpectedPulsewidthNs, float a_fMinEdgeSeparationNs, float a_fMaxEdgeJitterNs, float a_fMaxAmplitudeJitterVolts)
{
	m_fVoltageDivision_volts = (a_fVoltageMax - a_fVoltageMin)/(float)a_iDigitizerResolution;
	m_fVoltageStart_volts = a_fVoltageMin;
	m_fTimeDivision_ns = 1.0/a_fSamplingFreqGHz;
	m_markers.m_iPulseThreshold = (a_fPulseThresholdVolts - m_fVoltageStart_volts) / m_fVoltageDivision_volts;
	m_markers.m_iEdgeThreshold = (a_fEdgeThresholdVolts - m_fVoltageStart_volts) / m_fVoltageDivision_volts;
	m_markers.m_iExpectedPulseWidth = a_fExpectedPulsewidthNs / m_fTimeDivision_ns;
	m_markers.m_iMinEdgeSeparation = a_fMinEdgeSeparationNs / m_fTimeDivision_ns;
	m_markers.m_iMaxEdgeJitter = a_fMaxEdgeJitterNs / m_fTimeDivision_ns;
	m_markers.m_iMaxAmplitudeJitter = a_fMaxAmplitudeJitterVolts / m_fVoltageDivision_volts;

	printf("PULSE_THRESHOLD: %d\n", m_markers.m_iPulseThreshold);
	printf("EDGE_THRESHOLD: %d\n", m_markers.m_iEdgeThreshold);
	printf("EXPECTED_PULSE_WIDTH: %d\n", m_markers.m_iExpectedPulseWidth);
	printf("MIN_EDGE_SEPARATION: %d\n", m_markers.m_iMinEdgeSeparation);
	printf("MAX_EDGE_JITTER: %d\n", m_markers.m_iMaxEdgeJitter);
	printf("MAX_AMPLITUDE_JITTER: %d\n", m_markers.m_iMaxAmplitudeJitter);

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
@param a_samplesVector - a vector of samples from a channel
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
		if (it < m_markers.m_iPulseThreshold /*PULSE_THRESHOLD*/)
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
		if (a_samplesVector[i] < m_markers.m_iEdgeThreshold /*EDGE_THRESHOLD*/)
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

	int window_end = i + m_markers.m_iExpectedPulseWidth ;//EXPECTED_PULSE_WIDTH;

	for ( ; i < window_end; i++)
	{
		if (a_samplesVector[i] < minValue)
		{
			minValue = a_samplesVector[i];
			minValueIndex = i;
		}
	}
	

	return std::make_tuple(SignalAnalyzer::Point(leadingEdgeIndex, m_markers.m_iEdgeThreshold, m_fVoltageStart_volts, m_fTimeDivision_ns, m_fVoltageDivision_volts), 
			SignalAnalyzer::Point(minValueIndex, minValue, m_fVoltageStart_volts, m_fTimeDivision_ns, m_fVoltageDivision_volts));
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
		printf("X=%d, ",	std::get<0>(p).GetXDiscrete());
		if(!std::get<0>(p).Exists())
		{
			continue;
		}
		if (std::get<0>(p).GetXDiscrete() <= iEarliestLeadingEdge)
		{
			iNextToEarliestLeadingEdge = iEarliestLeadingEdge;
			iEarliestLeadingEdge = std::get<0>(p).GetXDiscrete();
			iEarliestLeadingEdgeIndex = i;
		}
		else if (std::get<0>(p).GetXDiscrete() < iNextToEarliestLeadingEdge)
		{
			iNextToEarliestLeadingEdge = std::get<0>(p).GetXDiscrete();
		}
	}
	printf("Earliest: %d, Next to earliest: %d\n" , iEarliestLeadingEdge, iNextToEarliestLeadingEdge);
	//The leading pulse is separated by enough time from the following pulses so that it can be considered as the original pulse.
	if ((iNextToEarliestLeadingEdge - iEarliestLeadingEdge) >= m_markers.m_iMinEdgeSeparation /*MIN_EDGE_SEPARATION*/)
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
		if(!(std::get<0>(m_markers.m_vChannelsEdgeAndMinimum[i]).Exists()))
		{
			continue;
		}

		if ((std::get<0>(m_markers.m_vChannelsEdgeAndMinimum[i]).GetXDiscrete() - iEarliestLeadingEdge) < m_markers.m_iMaxEdgeJitter /*MAX_EDGE_JITTER*/)
		{
			if (std::get<1>(m_markers.m_vChannelsEdgeAndMinimum[i]).GetYDiscrete() < minPulseValue)
			{
				minPulseValue = std::get<1>(m_markers.m_vChannelsEdgeAndMinimum[i]).GetYDiscrete();
			}
			vChannelsWithBunchedPulses.push_back(i);
		}
	}
	
	//look for bunched minima of the pulse within the window MAX_AMPLITUDE_JITTER upwards from the lowest minimum:
	for (auto& iChannelNum: vChannelsWithBunchedPulses)
	{
		if ((std::get<1>(m_markers.m_vChannelsEdgeAndMinimum[iChannelNum]).GetYDiscrete() - minPulseValue) <= m_markers.m_iMaxAmplitudeJitter /*MAX_AMPLITUDE_JITTER*/)
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
