#include "Event.h"
#include "Exception.h"
#include "EventConstants.h"


Event::Event(char* a_pData, int a_iSize)
{
	Parse(a_pData, a_iSize);
}

void Event::Parse(char* a_pData, int a_iSize)
{
	a_iSize += EVENT_HEADER_SIZE;
	EXTRACT_VALUE(m_iEventSizeBytes, a_pData, a_iSize, OFFSET_EVENT_SIZE, MASK_EVENT_SIZE, SHL_EVENT_SIZE);
	m_iEventSizeBytes *= EVENT_SIZE_GRANULARITY_BYTES;
	
	EXTRACT_VALUE(m_iGroupMask, a_pData, a_iSize, OFFSET_GROUP_MASK, MASK_GROUP_MASK, SHL_GROUP_MASK);
	EXTRACT_VALUE(m_iPattern, a_pData, a_iSize, OFFSET_PATTERN, MASK_PATTERN, SHL_PATTERN);	
	EXTRACT_VALUE(m_iBoardId, a_pData, a_iSize, OFFSET_BOARD_ID, MASK_BOARD_ID, SHL_BOARD_ID);
	EXTRACT_VALUE(m_iEventCounter, a_pData, a_iSize, OFFSET_EVENT_COUNTER, MASK_EVENT_COUNTER, SHL_EVENT_COUNTER);
	EXTRACT_VALUE(m_iEventTimeTag, a_pData, a_iSize, OFFSET_TIME_TAG, MASK_TIME_TAG, SHL_TIME_TAG);
	
	//read the four groups of events and populate the samples vector for each channel:
	int iGroupSizeAccumulator = 0;
	m_vGroupEvents.resize(NUMBER_OF_GROUPS);
	for (int iGroupCount = 0; iGroupCount < NUMBER_OF_GROUPS; iGroupCount++)
	{
		if ((m_iGroupMask & (GROUP_INITIAL_MASK << iGroupCount)))
		{
			//printf("group %d active\n", iGroupCount);
			//group number iGroupCount is active and contains data
			GroupEvent ev;
			ev.m_vChannelSamples.resize(NUMBER_OF_CHANNELS_IN_GROUP);
			//printf ("new size of m_vChannelSamples: %d\n", ev.m_vChannelSamples.size());
			int iOffset = OFFSET_START_OF_GROUP_LIST + iGroupSizeAccumulator;
			EXTRACT_VALUE(ev.m_iSizeOfEachChannelSamples, a_pData, a_iSize, iOffset + OFFSET_SIZE_OF_CHANNELS, MASK_SIZE_OF_CHANNELS, SHL_SIZE_OF_CHANNELS);
			ev.m_iSizeOfEachChannelSamples /= NUMBER_OF_DWORDS_IN_SAMPLE;
			EXTRACT_VALUE(ev.m_iTR, a_pData, a_iSize, iOffset + OFFSET_TR, MASK_TR, SHL_TR);	
			EXTRACT_VALUE(ev.m_iFrequency, a_pData, a_iSize, iOffset + OFFSET_FREQUENCY, MASK_FREQUENCY, SHL_FREQUENCY);
			EXTRACT_VALUE(ev.m_iStartIndexCell, a_pData, a_iSize, iOffset + OFFSET_START_INDEX_CELL, MASK_START_INDEX_CELL, SHL_START_INDEX_CELL);
			iOffset += DWORD_SIZE_BYTES;
			for (int iSampleCount = 0; iSampleCount < ev.m_iSizeOfEachChannelSamples; iSampleCount++)
			{
				int iChanValue = 0;
				int iChanValueHi = 0;
				int iChanValueLo = 0;
				
				//channel 0
				iOffset += NUMBER_OF_DWORDS_IN_SAMPLE * DWORD_SIZE_BYTES;
				EXTRACT_VALUE(iChanValue, a_pData, a_iSize, iOffset + OFFSET_CHANNEL_0, MASK_CHANNEL_0, SHL_CHANNEL_0);
				ev.m_vChannelSamples[0].push_back(iChanValue);
				//printf("CHANNEL SAMPLES at 0 size is %d\n", ev.m_vChannelSamples[0].size());
				
				//channel 1
				EXTRACT_VALUE(iChanValue, a_pData, a_iSize, iOffset + OFFSET_CHANNEL_1, MASK_CHANNEL_1, SHL_CHANNEL_1);
				ev.m_vChannelSamples[1].push_back(iChanValue);
				
				//channel 2 - fragmented to two dwords
				EXTRACT_VALUE(iChanValueLo, a_pData, a_iSize, iOffset + OFFSET_CHANNEL_2_LO, MASK_CHANNEL_2_LO, SHL_CHANNEL_2_LO);	
				EXTRACT_VALUE(iChanValueHi, a_pData, a_iSize, iOffset + OFFSET_CHANNEL_2_HI, MASK_CHANNEL_2_HI, SHL_CHANNEL_2_HI);
				iChanValue = (iChanValueLo >> RECONSTRUCT_SHIFT_CHANNEL_2_LO) | (iChanValueHi << RECONSTRUCT_SHIFT_CHANNEL_2_HI);
				ev.m_vChannelSamples[2].push_back(iChanValue);

				//channel 3
				EXTRACT_VALUE(iChanValue, a_pData, a_iSize, iOffset + OFFSET_CHANNEL_3, MASK_CHANNEL_3, SHL_CHANNEL_3);
				ev.m_vChannelSamples[3].push_back(iChanValue);

				//channel 4
				EXTRACT_VALUE(iChanValue, a_pData, a_iSize, iOffset + OFFSET_CHANNEL_4, MASK_CHANNEL_4, SHL_CHANNEL_4);
				ev.m_vChannelSamples[4].push_back(iChanValue);

				//channel 5 - fragmented to two dwords
				EXTRACT_VALUE(iChanValueLo, a_pData, a_iSize, iOffset + OFFSET_CHANNEL_5_LO, MASK_CHANNEL_5_LO, SHL_CHANNEL_5_LO);
				EXTRACT_VALUE(iChanValueHi, a_pData, a_iSize, iOffset + OFFSET_CHANNEL_5_HI, MASK_CHANNEL_5_HI, SHL_CHANNEL_5_HI);
				iChanValue = (iChanValueLo >> RECONSTRUCT_SHIFT_CHANNEL_5_LO) | (iChanValueHi << RECONSTRUCT_SHIFT_CHANNEL_5_HI);
				ev.m_vChannelSamples[5].push_back(iChanValue);

				//channel 6
				EXTRACT_VALUE(iChanValue, a_pData, a_iSize, iOffset + OFFSET_CHANNEL_6, MASK_CHANNEL_6, SHL_CHANNEL_6);
				ev.m_vChannelSamples[6].push_back(iChanValue);

				//channel 7
				EXTRACT_VALUE(iChanValue, a_pData, a_iSize, iOffset + OFFSET_CHANNEL_7, MASK_CHANNEL_7, SHL_CHANNEL_7);
				ev.m_vChannelSamples[7].push_back(iChanValue);
			}
			//printf ("ev.m_vChannelSamples[0] is of size %d", ev.m_vChannelSamples[0].size());
			int iSizeIncrement =  ev.m_iSizeOfEachChannelSamples * NUMBER_OF_DWORDS_IN_SAMPLE * DWORD_SIZE_BYTES;
			if (ev.m_iTR)
			{
				iSizeIncrement += ev.m_iSizeOfEachChannelSamples * BITS_PER_SAMPLE/BITS_PER_BYTE;
			}
			iSizeIncrement += TRIGGER_TIME_TAG_SIZE + EVENT_DESCRIPTION_SIZE; 
				
			iGroupSizeAccumulator += iSizeIncrement; 

			m_vGroupEvents[iGroupCount] = ev;
			//printf("m_vGroupEvents has %d entries. First one is of size %d\n", m_vGroupEvents.size(), m_vGroupEvents[0].m_vChannelSamples.size());			
		}
	}		
}
