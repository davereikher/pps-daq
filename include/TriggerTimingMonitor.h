#pragma once
#include <memory>
#include <chrono>
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "TimingMonitorBase.h"

using namespace std::chrono;

class TriggerTimingMonitor: public TimingMonitorBase
{
public:
	TriggerTimingMonitor(milliseconds a_periodDuration);
};
