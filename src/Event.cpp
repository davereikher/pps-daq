#include "Event.h"

#define EXTRACT_VALUE(data, offset, mask, shift_left) (*((unsigned int *)(data+(offset))) & mask) >> shift_left

//masks for extraction of fields from dwords
#define MASK_EVENT_SIZE			0x0fffffff
#define MASK_GROUP_MASK			0x0000000f
#define MASK_PATTERN			0x003fff00
#define MASK_BOARD_ID			0xf8000000
#define MASK_EVENT_COUNTER		0x003fffff
#define MASK_TIME_TAG			0xffffffff
#define MASK_SIZE_OF_CHANNELS		0x00000fff
#define MASK_TR				0x00001000
#define MASK_FREQUENCY			0x00030000
#define MASK_START_INDEX_CELL		0x3ff00000
#define MASK_CHANNEL_0			0x00000fff
#define MASK_CHANNEL_1			0x00fff000
#define MASK_CHANNEL_2_LO		0xff000000
#define MASK_CHANNEL_2_HI		0x0000000f
#define MASK_CHANNEL_3			0x0000fff0
#define MASK_CHANNEL_4			0x0fff0000
#define MASK_CHANNEL_5_LO		0xf0000000
#define MASK_CHANNEL_5_HI		0x000000ff
#define MASK_CHANNEL_6			0x000fff00
#define MASK_CHANNEL_7			0xfff00000

//how much to shift left each field to get it's value
#define SHL_EVENT_SIZE			0
#define SHL_GROUP_MASK			0
#define SHL_PATTERN			8
#define SHL_BOARD_ID			27
#define SHL_EVENT_COUNTER		0
#define SHL_TIME_TAG			0
#define SHL_SIZE_OF_CHANNELS		0
#define SHL_TR				12
#define SHL_FREQUENCY			16
#define SHL_START_INDEX_CELL		20
#define SHL_CHANNEL_0			0
#define SHL_CHANNEL_1			12
#define SHL_CHANNEL_2_LO		0
#define SHL_CHANNEL_2_HI		0
#define SHL_CHANNEL_3			4
#define SHL_CHANNEL_4			16
#define SHL_CHANNEL_5_LO		0
#define SHL_CHANNEL_5_HI		0
#define SHL_CHANNEL_6			8
#define SHL_CHANNEL_7			20

//how much to shift the extracted low and high parts of the sample in a given channel before ORing the two parts to get the final value of that sample
#define RECONSTRUCT_SHIFT_CHANNEL_2_LO	24
#define RECONSTRUCT_SHIFT_CHANNEL_2_HI	8
#define RECONSTRUCT_SHIFT_CHANNEL_5_LO	28
#define RECONSTRUCT_SHIFT_CHANNEL_5_HI	0

//The following offsets are relative to the beginning of the whole event
#define OFFSET_EVENT_SIZE		0
#define OFFSET_GROUP_MASK		4
#define OFFSET_PATTERN			4
#define OFFSET_BOARD_ID			4
#define OFFSET_EVENT_COUNTER		8
#define OFFSET_TIME_TAG			12
#define OFFSET_START_OF_GROUP_LIST	16

//The following offsets are relative to the beginning of each group event
#define OFFSET_SIZE_OF_CHANNELS		0
#define OFFSET_TR			0
#define OFFSET_FREQUENCY		0
#define OFFSET_START_INDEX_CELL		0

//the following offsets are relative to the beginning of each sample
#define OFFSET_CHANNEL_0		0
#define OFFSET_CHANNEL_1		0
#define OFFSET_CHANNEL_2_LO		0
#define OFFSET_CHANNEL_2_HI		4
#define OFFSET_CHANNEL_3		4
#define OFFSET_CHANNEL_4		4
#define OFFSET_CHANNEL_5_LO		4
#define OFFSET_CHANNEL_5_HI		8
#define OFFSET_CHANNEL_6		8
#define OFFSET_CHANNEL_7		8

#define EVENT_SIZE_GRANULARITY_BYTES 	4 

#define NUMBER_OF_GROUPS		4
#define NUMBER_OF_CHANNELS_IN_GROUP	8
#define NUMBER_OF_DWORDS_IN_SAMPLE	3
#define DWORD_SIZE_BYTES		sizeof(unsigned int)

#define GROUP_INITIAL_MASK		0x00000001

void Event::Parse(unsigned char* a_pData, int a_iSize)
{
	m_iEventSizeBytes = EXTRACT_VALUE(a_pData, OFFSET_EVENT_SIZE, MASK_EVENT_SIZE, SHL_EVENT_SIZE) * EVENT_SIZE_GRANULARITY_BYTES;
	m_iGroupMask = EXTRACT_VALUE(a_pData, OFFSET_GROUP_MASK, MASK_GROUP_MASK, SHL_GROUP_MASK);
	m_iPattern = EXTRACT_VALUE(a_pData, OFFSET_PATTERN, MASK_PATTERN, SHL_PATTERN);
	m_iBoardId = EXTRACT_VALUE(a_pData, OFFSET_BOARD_ID, MASK_BOARD_ID, SHL_BOARD_ID);
	m_iEventCounter = EXTRACT_VALUE(a_pData, OFFSET_EVENT_COUNTER, MASK_EVENT_COUNTER, SHL_EVENT_COUNTER);
	m_iEventTimeTag = EXTRACT_VALUE(a_pData, OFFSET_TIME_TAG, MASK_TIME_TAG, SHL_TIME_TAG);
	
	//read the four groups of events and populate the samples vector for each channel:
	int group_size_accumulator = 0;
	m_vGroupEvents.resize(NUMBER_OF_GROUPS);
	for (int group_count = 0; group_count < NUMBER_OF_GROUPS; group_count++)
	{
		if ((m_iGroupMask & (GROUP_INITIAL_MASK << group_count)))
		{
			//group number group_count is active and contains data
			GroupEvent ev;
			ev.m_vChannelSamples.resize(NUMBER_OF_CHANNELS_IN_GROUP);
			int offset = OFFSET_START_OF_GROUP_LIST + group_size_accumulator;
			ev.m_iSizeOfEachChannelSamples = EXTRACT_VALUE(a_pData, offset + OFFSET_SIZE_OF_CHANNELS, MASK_SIZE_OF_CHANNELS, SHL_SIZE_OF_CHANNELS) / NUMBER_OF_DWORDS_IN_SAMPLE;
			ev.m_iTR = EXTRACT_VALUE(a_pData, offset + OFFSET_TR, MASK_TR, SHL_TR);
			ev.m_iFrequency = EXTRACT_VALUE(a_pData, offset + OFFSET_FREQUENCY, MASK_FREQUENCY, SHL_FREQUENCY);
			ev.m_iStartIndexCell = EXTRACT_VALUE(a_pData, offset + OFFSET_START_INDEX_CELL, MASK_START_INDEX_CELL, SHL_START_INDEX_CELL);
			offset += DWORD_SIZE_BYTES;
			for (int sample_count = 0; sample_count < ev.m_iSizeOfEachChannelSamples; sample_count++)
			{
				int chan_value = 0;
				int chan_value_hi = 0;
				int chan_value_lo = 0;
				
				//channel 0
				offset += sample_count * NUMBER_OF_DWORDS_IN_SAMPLE * DWORD_SIZE_BYTES;
				chan_value = EXTRACT_VALUE(a_pData, offset + OFFSET_CHANNEL_0, MASK_CHANNEL_0, SHL_CHANNEL_0);
				ev.m_vChannelSamples[0].push_back(chan_value);

				//channel 1
				chan_value = EXTRACT_VALUE(a_pData, offset + OFFSET_CHANNEL_1, MASK_CHANNEL_1, SHL_CHANNEL_1);
				ev.m_vChannelSamples[1].push_back(chan_value);

				//channel 2 - fragmented to two dwords
				chan_value_lo = EXTRACT_VALUE(a_pData, offset + OFFSET_CHANNEL_2_LO, MASK_CHANNEL_2_LO, SHL_CHANNEL_2_LO);	
				chan_value_hi = EXTRACT_VALUE(a_pData, offset + OFFSET_CHANNEL_2_HI, MASK_CHANNEL_2_HI, SHL_CHANNEL_2_HI);
				chan_value = (chan_value_lo >> RECONSTRUCT_SHIFT_CHANNEL_2_LO) | (chan_value_hi << RECONSTRUCT_SHIFT_CHANNEL_2_HI);
				ev.m_vChannelSamples[2].push_back(chan_value);

				//channel 3
				chan_value = EXTRACT_VALUE(a_pData, offset + OFFSET_CHANNEL_3, MASK_CHANNEL_3, SHL_CHANNEL_3);
				ev.m_vChannelSamples[3].push_back(chan_value);

				//channel 4
				chan_value = EXTRACT_VALUE(a_pData, offset + OFFSET_CHANNEL_4, MASK_CHANNEL_4, SHL_CHANNEL_4);
				ev.m_vChannelSamples[4].push_back(chan_value);

				//channel 5 - fragmented to two dwords
				chan_value_lo = EXTRACT_VALUE(a_pData, offset + OFFSET_CHANNEL_5_LO, MASK_CHANNEL_5_LO, SHL_CHANNEL_5_LO);
				chan_value_hi = EXTRACT_VALUE(a_pData, offset + OFFSET_CHANNEL_5_HI, MASK_CHANNEL_5_HI, SHL_CHANNEL_5_HI);
				chan_value = (chan_value_lo >> RECONSTRUCT_SHIFT_CHANNEL_5_LO) | (chan_value_hi << RECONSTRUCT_SHIFT_CHANNEL_5_HI);
				ev.m_vChannelSamples[5].push_back(chan_value);

				//channel 6
				chan_value = EXTRACT_VALUE(a_pData, offset + OFFSET_CHANNEL_6, MASK_CHANNEL_6, SHL_CHANNEL_6);
				ev.m_vChannelSamples[6].push_back(chan_value);

				//channel 7
				chan_value = EXTRACT_VALUE(a_pData, offset + OFFSET_CHANNEL_7, MASK_CHANNEL_7, SHL_CHANNEL_7);
				ev.m_vChannelSamples[7].push_back(chan_value);	
			}
			m_vGroupEvents.push_back(ev);
		}
	}		
}
