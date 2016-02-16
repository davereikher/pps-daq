#include <stdio.h>
#include "CAENDigitizer.h"
#include "ProprietaryUtils.h"

unsigned int Proprietary1742Utils::GetEventSize(int a_iHandle)
{
	unsigned int data = 0;
	int ret = CAEN_DGTZ_ReadRegister(a_iHandle, CAEN_DTGZ_EVENT_SIZE_ADD, &data);
	if (ret != 0)
	{
		printf("GetEventSize (ReadRegister) failed with error %d\n", ret);
		return -1;
	}
	return data;
}

unsigned int Proprietary1742Utils::GetBoardType(int a_iHandle)
{
	unsigned int data = 0;
	int ret = CAEN_DGTZ_ReadRegister(a_iHandle, CAEN_DGTZ_BOARD_INFO_ADD, &data);
	if (ret != 0)
	{
		printf("GetBoardType (ReadRegister) failed with error %d\n", ret);
		return -1;
	}
	return data & 0x000000FF;
}

unsigned int Proprietary1742Utils::GetGroupEnableMask(int a_iHandle)
{
	unsigned int data = 0;
	int ret = CAEN_DGTZ_ReadRegister(a_iHandle, CAEN_DGTZ_CH_ENABLE_ADD, &data);
	if (ret != 0)
	{
		printf("GetGroupEnableMask (ReadRegister) failed with error %d\n", ret);
		return -1;
	}
	return data;
}

unsigned int Proprietary1742Utils::GetPostTriggerSetting(int a_iHandle)
{
	unsigned int data = 0;
	int ret = CAEN_DGTZ_ReadRegister(a_iHandle, CAEN_DGTZ_POST_TRIG_ADD, &data);
	if (ret != 0)
	{
		printf("GetPostTriggerSetting (ReadRegister) failed with error %d\n", ret);
		return -1;
	}
	return data;
}

//This returns -1 for some reason...
unsigned int Proprietary1742Utils::GetSamplingFrequency(int a_iHandle)
{
	unsigned int data = 0;
	int ret = CAEN_DGTZ_ReadRegister(a_iHandle, CAEN_DGTZ_DRS4_FREQUENCY_REG_WRITE, &data);
	if (ret != 0)
	if (ret != 0)
	{
		printf("GetSamplingFrequency (ReadRegister) failed with error %d\n", ret);
		return -1;
	}
	return data;
}



int Proprietary1742Utils::SetGroupEnableMask(int a_iHandle, bool a_iGroup0, bool a_iGroup1, bool a_iGroup2, bool a_iGroup3)
{
	unsigned int data = 0;
	data |= a_iGroup0 | (a_iGroup1 << 1) | (a_iGroup2 << 2) | (a_iGroup3 << 3);
	int ret = CAEN_DGTZ_WriteRegister(a_iHandle, CAEN_DGTZ_CH_ENABLE_ADD, data);
	if (ret != 0)
	{
		return ret;
	}

	return 0;
}

int Proprietary1742Utils::SetSamplingFrequency(int a_iHandle, unsigned int a_iFrequency)
{
	unsigned int data = 0;
	data |= a_iFrequency & 0x00000003;
	int ret = CAEN_DGTZ_WriteRegister(a_iHandle, CAEN_DGTZ_DRS4_FREQUENCY_REG_WRITE, data);
	if (ret != 0)
	{
		return ret;
	}
	return 0;
}


