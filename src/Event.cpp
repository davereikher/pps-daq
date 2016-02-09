#include "Event.h"

#define EXTRACT_VALUE(data, offset, mask, shift_left) (*((unsigned int *)(data+(offset))) & mask) >> shift_left

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

#define SHIFT_LEFT_EVENT_SIZE		0
#define SHIFT_LEFT_GROUP_MASK		0
#define SHIFT_LEFT_PATTERN		8
#define SHIFT_LEFT_BOARD_ID		27
#define SHIFT_LEFT_EVENT_COUNTER	0
#define SHIFT_LEFT_TIME_TAG		0
#define SHIFT_LEFT_SIZE_OF_CHANNELS	0
#define SHIFT_LEFT_TR			12
#define SHIFT_LEFT_FREQUENCY		16
#define SHIFT_LEFT_START_INDEX_CELL	20

//The following offsets are relative to the start of the whole event
#define OFFSET_EVENT_SIZE		0
#define OFFSET_GROUP_MASK		32
#define OFFSET_PATTERN			32
#define OFFSET_BOARD_ID			32
#define OFFSET_EVENT_COUNTER		64
#define OFFSET_TIME_TAG			96
#define OFFSET_START_OF_GROUP_LIST	128

//The following offsets are relative to the start of each group event
#define OFFSET_SIZE_OF_CHANNELS		0
#define OFFSET_TR			0
#define OFFSET_FREQUENCY		0
#define OFFSET_START_INDEX_CELL		0

#define EVENT_SIZE_GRANULARITY_BYTES 	4 

#define NUMBER_OF_GROUPS		4

#define GROUP_INITIAL_MASK		0x00000001

void Event::Parse(unsigned char* a_pData, int a_iSize)
{
	m_iEventSizeBytes = EXTRACT_VALUE(a_pData, OFFSET_EVENT_SIZE, MASK_EVENT_SIZE, SHIFT_LEFT_EVENT_SIZE) * EVENT_SIZE_GRANULARITY_BYTES;
	m_iGroupMask = EXTRACT_VALUE(a_pData, OFFSET_GROUP_MASK, MASK_GROUP_MASK, SHIFT_LEFT_GROUP_MASK);
	m_iPattern = EXTRACT_VALUE(a_pData, OFFSET_PATTERN, MASK_PATTERN, SHIFT_LEFT_PATTERN);
	m_iBoardId = EXTRACT_VALUE(a_pData, OFFSET_BOARD_ID, MASK_BOARD_ID, SHIFT_LEFT_BOARD_ID);
	m_iEventCounter = EXTRACT_VALUE(a_pData, OFFSET_EVENT_COUNTER, MASK_EVENT_COUNTER, SHIFT_LEFT_EVENT_COUNTER);
	m_iEventTimeTag = EXTRACT_VALUE(a_pData, OFFSET_TIME_TAG, MASK_TIME_TAG, SHIFT_LEFT_TIME_TAG);
	
	//read the four groups of events and populate the samples vector for each channel:
	int group_size_accumulator = 0;
	for (int group_count = 0; group_count < NUMBER_OF_GROUPS; group_count++)
	{
		if ((m_iGroupMask & (GROUP_INITIAL_MASK << group_count)))
		{
			//group number group_count is active and contains data
			GroupEvent ev;
			int offset = OFFSET_START_OF_GROUP_LIST + group_size_accumulator;
			ev.m_iSizeOfEachChannel = EXTRACT_VALUE(a_pData, offset + OFFSET_SIZE_OF_CHANNELS, MASK_SIZE_OF_CHANNELS, SHIFT_LEFT_SIZE_OF_CHANNELS);
			ev.m_iTR = EXTRACT_VALUE(a_pData, offset + OFFSET_TR, MASK_TR, SHIFT_LEFT_TR);
			ev.m_iFrequency = EXTRACT_VALUE(a_pData, offset + OFFSET_FREQUENCY, MASK_FREQUENCY, SHIFT_LEFT_FREQUENCY);
			ev.m_iStartIndexCell = EXTRACT_VALUE(a_pData, offset + OFFSET_START_INDEX_CELL, MASK_START_INDEX_CELL, SHIFT_LEFT_START_INDEX_CELL);
			
		}
	}		
}
