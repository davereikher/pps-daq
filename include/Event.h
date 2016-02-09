#pragma once
#include <vector>
#include "CAENDigitizer.h"

//This class represents the entire event coming from the digitizer (it includes the 4 group events)
class Event
{
private:

	//This struct represents each one of the 4 group events
	struct GroupEvent
	{
		unsigned int m_iStartIndexCell;
		unsigned int m_iFrequency;
		unsigned int m_iTR;
		unsigned int m_iSizeOfEachChannelSamples; //The size is the number of 4 byte words (each sample is 3 4 byte words containing all channels). This is basically the number of samples in each channel.
		std::vector<std::vector<unsigned short> > m_vChannelSamples; //index is the channel number (0-7) in the current group, inner vector is the vector of samples
	};
public:
	Event(unsigned char* a_pData, int a_iSize);
	void Parse(unsigned char* a_pData, int a_iSize);
private:
	unsigned int m_iEventSizeBytes;
	unsigned int m_iBoardId;
	unsigned int m_iPattern;
	unsigned int m_iGroupMask;
	unsigned int m_iEventCounter;
	unsigned int m_iEventTimeTag;
	std::vector<GroupEvent> m_vGroupEvents; //index is the group number
};
