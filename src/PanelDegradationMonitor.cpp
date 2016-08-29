#include "PanelDegradationMonitor.h"
#include "Logger.h"

PanelDegradationMonitor::PanelDegradationMonitor(milliseconds a_periodDuration, std::string a_sPanelName):
TimingMonitorBase(a_periodDuration, 1, "Panel Degradation Monitor", "Panel Hit Rate", "Intervals Between Hits", (a_sPanelName + "_PanelDegr").c_str(), (a_sPanelName + "_TimingHist").c_str())
{}


