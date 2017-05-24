#pragma once
#include <memory>
#include <chrono>
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "ChannelCountRateMonitorBase.h"

using namespace std::chrono;

class ChannelCountRateMonitor: public ChannelCountRateMonitorBase
{
public:
	ChannelCountRateMonitor(milliseconds a_periodDuration);
};
