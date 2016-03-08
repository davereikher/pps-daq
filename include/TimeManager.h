#pragma once
#include <chrono>

using namespace std;
using namespace std::chrono;

typedef duration<int64_t, ratio<20, 1000*1000*1000> > DigitizerTicks_t;

class TimeManager
{
public:
	static nanoseconds ConvertPrecisionTime(int a_iPreciseDurationTicks);

private:
	static DigitizerTicks_t m_maxTicks;
	static time_point<high_resolution_clock> m_lastTimePoint;
	static time_point<high_resolution_clock> m_totalStartTime;
	static int m_iLastPreciseDurationTicks;
};
