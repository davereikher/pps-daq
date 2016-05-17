#pragma once
#include <vector>
#include <chrono>
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "Types.h"
#include "TimingMonitorBase.h"

using namespace std::chrono;

class PanelDegradationMonitor: public TimingMonitorBase
{
public:	
	PanelDegradationMonitor(milliseconds a_periodDuration, std::string a_sPanelName);
};
