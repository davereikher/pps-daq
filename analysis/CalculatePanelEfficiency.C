#include <cfloat>
#define MINIMUM_VALUE_FOR_SUIGNAL 1950

std::vector <std::vector<int>> gvPanelChannelRanges;

/*
We loop over all events. For each event, and for each range of channels (each range corresponds to a panel), we look for the channel that was hit by the following procedure:
	- Define a threshold PULSE_THRESHOLD, so that if a sample in a channel is found below it, this channel will be considered to have a pulse.
	- Define a threshold EDGE_THRESHOLD, so that the time at which the signal in a channel first reaches it will be the time of the leading edge of the first pulse in that channel.
	- Define a threshold EXPECTED_PULSE_WIDTH, which is the size of the window after the leading edge of a pulse to look for the minimum value of the signal in (the sample with the lowest numeric value).
	- Define a threshold MIN_EDGE_SEPARATION, which is the minimum amount of time the first detected pulse in a range of channels arrives before the following one in that same range for that pulse to be considered as the original one.
	- Define a threshold MAX_EDGE_JITTER, which is the length of the window from the leading edge of the first pulse inside which pulses are considered to be "inseperable", that is, we cannot say just by the leading edge position which pulse comes first.
	- For each event:
		- For each range:
			- Map the vectors of samples associated with each channel in that range to a vector of pairs (leading_edge, pulse_amplitude)
			- In the vector of pairs find the lowest and the next to lowest value of leading_edge. 
			- If these values are farther away than MIN_EDGE_SEPARATION:
				- Take the one with the lowest value as the signal.
			- Otherwise:
				- Take all channels the leading_edge on which are bundled within a distance MAX_EDGE_JITTER from the first leading_edge and find the channel with the largest pulse_amplitude. This is the channel with the original pulse.
*/

#define NO_PULSE -1
#define PULSE_THRESHOLD -1950 //TODO: set these values
#define EDGE_THRESHOLD -1000


std::pair<int, int> FindLeadingEdgeAndPulseAmplitude(std::vector<float> a_samplesVector)
{
	std::pair<int, int> pair; //leading edge and pulse threshold pair
	float minValue = FLT_MAX; 
	int minValueIndex = -1;
	int leading_edge = -1;
	
	for (int i = 0; i < a_samplesVector.size(), i++)
	{
		if (a_samplesVector[i] < EDGE_THRESHOLD)
		{
		//Reached a leading edge. This is a pulse on this channel (not necessarily the original pulse). 
		//Look inside a window EXPECTED_PULSE_WIDTH wide for the lowest value
			leading_edge = i;
			break;
		}
	}

	if (i == a_samplesVector.size())
	{
		//no leading edge found => no pulse
		pair.first = NO_PULSE;
		pair.second = NO_PULSE;
		return pair;
	}

	int window_end = i + EXPECTED_PULSE_WIDTH;

	for ( ; i < window_end; i++)
	{
		if (a_samplesVector[i] < minValue):
		{
			minValue = a_samplesVector[i];
		}
	}
	
	pair.first = leading_edge;
	pair.second = a_samplesVector;

	return pair;
}

void FindOriginalPulseInChannelRange(std::vector<std::vector<float> >& a_vAllChannels, std::vector<int>& a_vRange)
{
	for (auto it = a_vRange.begin(); it != a_vRange.end(); ++it)
	{
		
	}
}
