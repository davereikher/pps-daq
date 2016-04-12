#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include "CAENDigitizer.h"
#include "Types.h"

using namespace std;
using namespace chrono;

class EventHandler
{
public:
	virtual ~EventHandler();
	void Handle(CAEN_DGTZ_X742_EVENT_t* a_pEvent, nanoseconds a_eventTime);
	virtual void DoHandle(nanoseconds a_eventTime) = 0;

private:

	void Transform(CAEN_DGTZ_X742_EVENT_t* a_pEvent);
	void PrintEventInfo(CAEN_DGTZ_EventInfo_t* p_eventInfo);
	bool IsEventEmpty();

protected:
	Channels_t m_vChannels;
};

