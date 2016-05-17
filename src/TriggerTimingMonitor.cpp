#include "TriggerTimingMonitor.h"
#include "Configuration.h"

TriggerTimingMonitor::TriggerTimingMonitor(milliseconds a_periodDuration):
TimingMonitorBase(a_periodDuration, Configuration::Instance().GetNumberOfEventsToDrawAfter(), "Trigger Timing Monitor", "Trigger Rate", "Intervals Between Triggers", "TriggerTiming", "TimingHist")
{}

