#pragma once

#include <memory>
#include "CAENDigitizer.h"
#include "EventHandler.h"

class DigitizerManager
{
public:
	DigitizerManager(EventHandler& a_handler);
	~DigitizerManager();
	void InitAndConfigure();
	void PrintBoardInfo();
	void Start();
	int Acquire();
private:
	int m_iHandle;
	CAEN_DGTZ_X742_EVENT_t* m_pEvent;
	EventHandler& m_eventHandler;
	char* m_pBuffer;	
	CAEN_DGTZ_EventInfo_t m_eventInfo;
};
