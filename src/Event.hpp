#pragma once

//This class represents the entire event coming from the digitizer (it includes the 4 group events)
class DigitizerEvent
{
private:

	//This class represents each one of the 4 group events
	class Event
	{
	private:
		enum Frequency
		{
			Freq5GSs, //(Giga samples/second)
			Freq2_5Gss,
		};
	private:
		unsigned int m_iStartIndexCell;
		unsigned int m_iFrequency;
	};
private:
	unsigned int m_iEventSize;
	unsigned int m_iBoardId;
	unsigned int m_iPattern;
	unsigned int m_iGroupMask;
	unsigned int m_iEventCounter;
	unsigned int m_iEventTimeTag;
	std::vector<std::vector<unsigned short>> m_iSamples; //index is the channel number
};
