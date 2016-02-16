#pragma once

class Proprietary1742Utils
{
public:

//	Getters:
	static unsigned int GetEventSize(int a_iHandle);
	static unsigned int GetBoardType(int a_iHandle);
	static unsigned int GetGroupEnableMask(int a_iHandle);
	static unsigned int GetPostTriggerSetting(int a_iHandle);
	static unsigned int GetSamplingFrequency(int a_iHandle);


//	Setters:
	static int SetGroupEnableMask(int a_iHandle, bool a_iGroup0, bool a_iGroup1, bool a_iGroup2, bool a_iGroup3);
	static int SetSamplingFrequency(int a_iHandle, unsigned int a_iFrequency); //Frequency is the CAEN_DGTZ_DRS4_Frequency_t enum
	
};
