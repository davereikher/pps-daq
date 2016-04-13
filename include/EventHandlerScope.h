#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include "Types.h"
#include "EventHandler.h"
#include "RangePlotter.h"

using namespace chrono;

class EventHandlerScope: public EventHandler
{
public:
	EventHandlerScope();
	virtual ~EventHandlerScope();
	void DoHandle(nanoseconds a_eventTime);

private:
	std::unique_ptr<RangePlotter> m_pRangePlotter;
	Range_t m_ranges;	
};

