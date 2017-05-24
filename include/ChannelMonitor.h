#pragma once
#include <memory>
#include <chrono>
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "ChannelMonitorBase.h"

using namespace std::chrono;

class ChannelMonitor: public ChannelMonitorBase
{
public:
	ChannelMonitor(milliseconds a_periodDuration);
};
