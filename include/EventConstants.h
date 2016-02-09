#pragma once

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


