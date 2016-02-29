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
#define PULSE_THRESHOLD 1700 //TODO: set these values
#define EDGE_THRESHOLD 1840
#define EXPECTED_PULSE_WIDTH 70
#define MIN_EDGE_SEPARATION 100
#define MAX_EDGE_JITTER 50
#define MAX_AMPLITUDE_JITTER 200

SignalAnalyzer::SignalAnalyzer()
{
	m_markers.m_iPulseThreshold = PULSE_THRESHOLD;
	m_markers.m_iEdgeThreshold = EDGE_THRESHOLD;
	m_markers.m_iExpectedPulseWidth = EXPECTED_PULSE_WIDTH;
	m_markers.m_iMinEdgeSeparation = MIN_EDGE_SEPARATION;
	m_markers.m_iMaxEdgeJitter = MAX_EDGE_JITTER;
	m_markers.m_iMaxAmplitudeJitter = MAX_AMPLITUDE_JITTER;
}


/**
Maps a vector of samples to a pair of values - the leading edge of the first occurence of a pulse and the lowest value of that pulse.
@param a_samplesVector - a vector of samples from a channel
@return A pair, where the first item is the location of the leading edge and the second one is the minimum of the pulse.
*/
std::pair<int, float> SignalAnalyzer::FindLeadingEdgeAndPulseHeight(std::vector<float>& a_samplesVector)
{
	std::pair<int, float> p; //leading edge and pulse threshold pair
	float minValue = FLT_MAX; 
	int leading_edge = -1;
	int i = -1;	

	for (i = 0; i < (int)a_samplesVector.size(); i++)
	{
		if (a_samplesVector[i] < EDGE_THRESHOLD)
		{
		//Reached a leading edge. This is a pulse on this channel (not necessarily the original pulse). 
		//Look inside a window EXPECTED_PULSE_WIDTH wide for the lowest value
			leading_edge = i;
			break;
		}
	}

	if (i == (int)a_samplesVector.size())
	{
		//no leading edge found => no pulse
		p.first = NO_PULSE_EDGE;
		p.second = NO_PULSE_MINIMUM_VALUE;
		return p;
	}

	int window_end = i + EXPECTED_PULSE_WIDTH;

	for ( ; i < window_end; i++)
	{
		if (a_samplesVector[i] < minValue)
		{
			minValue = a_samplesVector[i];
		}
	}
	
	p.first = leading_edge;
	p.second = minValue;

	return p;
}

/**
Analyzes a range of the channels to find the original pulse.
- For each event:
	- For each range:
		- Map the vectors of samples associated with each channel in that range to a vector of pairs (leading_edge, min_pulse_value)
		- In the vector of pairs find the lowest and the next to lowest value of leading_edge. 
		- If these values are farther away than MIN_EDGE_SEPARATION:
			- Take the one with the lowest value as the signal.
		- Otherwise:
			- Take all channels the leading_edge on which are bundled within a distance MAX_EDGE_JITTER from the first leading_edge and find the channel with the lowest min_pulse_value.
			- Return all channel numbers the min_pulse_value of which are bundled withing a window of size MAX_AMPLITUDE_JITTER upwards from the lowest min_pulse_value.
The result, a vector of channels containing the original pulse (ideally there will be just one item), is placed in m_markers.m_vChannelsWithPulse. The pairs returned by FindLeadingEdgeAndPulseHeight are also stored in m_markers.m_vChannelsEdgeAndMinimum.

@param a_vAllChannels - a vector of all 32 channels, where each item is a vector of samples from that channel.
@param a_vRange - a vector of indices of the interesting channels in the first parameter.
*/
void SignalAnalyzer::FindOriginalPulseInChannelRange(std::vector<std::vector<float> >& a_vAllChannels, std::vector<int>& a_vRange)
{
	//Each pair represents a pulse. The first value is the leading edge time and the second is the lowest value of the pulse.
//	std::vector<std::pair<int, float> > pairs;

	//Get all channels in the provided range, find the leading edge and minimum value of the pulses and store the result in a vector of pairs. Also, find the earliest and next-to-earliest leading edge in the channels in the range.
	int iEarliestLeadingEdge = INT_MAX;
	int iNextToEarliestLeadingEdge = INT_MAX;
	int iEarliestLeadingEdgeIndex = -1;
	for (int i = 0; i < (int)a_vRange.size(); i++)
	{
		std::pair<int, float> p;
		p = FindLeadingEdgeAndPulseHeight(a_vAllChannels[a_vRange[i]]);
		m_markers.m_vChannelsEdgeAndMinimum.push_back(p);

		if (p.first <= iEarliestLeadingEdge)
		{
			iNextToEarliestLeadingEdge = iEarliestLeadingEdge;
			iEarliestLeadingEdge = p.first;
			iEarliestLeadingEdgeIndex = i;
		}
		else if (p.first < iNextToEarliestLeadingEdge)
		{
			iNextToEarliestLeadingEdge = p.first;
		}
	}

	//The leading pulse is separated by enough time from the following pulses so that it can be considered as the original pulse.
	if ((iNextToEarliestLeadingEdge - iEarliestLeadingEdge) >= MIN_EDGE_SEPARATION)
	{
		m_markers.m_vChannelsWithPulse.push_back(a_vRange[iEarliestLeadingEdgeIndex]);
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
		if ((m_markers.m_vChannelsEdgeAndMinimum[i].first - iEarliestLeadingEdge) < MAX_EDGE_JITTER)
		{
			if (m_markers.m_vChannelsEdgeAndMinimum[i].second < minPulseValue)
			{
				minPulseValue = m_markers.m_vChannelsEdgeAndMinimum[i].second;
			}
			vChannelsWithBunchedPulses.push_back(i);
		}
	}
	
	//look for bunched minima of the pulse within the window MAX_AMPLITUDE_JITTER upwards from the lowest minimum:
	for (auto& iChannelNum: vChannelsWithBunchedPulses)
	{
		if ((m_markers.m_vChannelsEdgeAndMinimum[iChannelNum].second - minPulseValue) <= MAX_AMPLITUDE_JITTER)
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
