#include <stdio.h>
#include "TimeManager.h"

#define MSB 0xFFFFFFFF00000000
#define LSB 0x00000000FFFFFFFF

//0x7FFFFFFF times 20 ns (about 42.9 seconds) is the highest possible value (without overflow) of the event time stamp from the digitizer. We want it to be less, to avoid bugs where the overflowed value of digitizer ticks is added to the current time point (around 35 seconds) - see implementation of ConvertPrecisionTime.
DigitizerTicks_t TimeManager::m_maxTicks(0x68FFFFFF);
int TimeManager::m_iLastPreciseDurationTicks = -1;
time_point<high_resolution_clock> TimeManager::m_lastTimePoint;
time_point<high_resolution_clock> TimeManager::m_totalStartTime;

/**
Converts the precise time stamp coming from the digitizer to a time duration from the beginning of the acquisition.
As long as the rates are no less than around 1/34 Hz, the resulting time stamp is going to be of digitizer-tick precision (20 ns precision) by taking the time stamp
from the digitizer and adding it to a counter. 
As soon as en event is farther apart from the previous event than about 35 seconds, the time stamp is taken from the pc clock from now on, since the event time stamp coming from the digitizer
is no longer reliable (in this simplified implementation). The PC clock is much less accurate (microsecond precision at best).
Note that if there is any kind of buffering in the pipelines from the digitizer to this code, this could be a major problem since the time at which the events arrive will be delayed by possibly more
than 35 seconds. This function should therefore run asap after an event arrives from the digitizer and further analysis should be performed in a separate thread.
This is also a problem for high trigger rates because they will pile up and cause the same problem. The trigger rates should not exceed a few Hz.

@param a_iPreciseDurationTicks
@return time from start of acquisition of type std::chrono::nanoseconds
*/
nanoseconds TimeManager::ConvertPrecisionTime(int a_iPreciseDurationTicks)
{
//TODO: implement a more elaborate time synchronization scheme by dividing the period of time from the current time point to the last one by the maximum precise duration and correcting by a_iPreciseDurationTicks
	time_point<high_resolution_clock> tp = high_resolution_clock::now();

//	a_iPreciseDurationTicks &= 0x7FFFFFFF;

	if (m_iLastPreciseDurationTicks == -1)
	{
		m_totalStartTime = tp;
		m_lastTimePoint = tp;
		m_iLastPreciseDurationTicks = a_iPreciseDurationTicks;
		return nanoseconds(0);
	}

/*	nanoseconds d = tp - m_lastTimePoint;
	
	if (d < m_maxTicks)
	{
		DigitizerTicks_t precisionTicks;
		if (a_iPreciseDurationTicks >= m_iLastPreciseDurationTicks) 
		{
			precisionTicks = DigitizerTicks_t(a_iPreciseDurationTicks - m_iLastPreciseDurationTicks);	
		}
		else
		{
			precisionTicks = DigitizerTicks_t(0x7FFFFFFF - m_iLastPreciseDurationTicks + a_iPreciseDurationTicks);	
			printf("OVERFLOW! precisionTicks is %llu\n", precisionTicks.count());
		}
		m_lastTimePoint += precisionTicks;
		m_iLastPreciseDurationTicks = a_iPreciseDurationTicks;
		return m_lastTimePoint - m_totalStartTime;
	}
*/
	m_lastTimePoint = tp;

	return m_lastTimePoint - m_totalStartTime;
}

