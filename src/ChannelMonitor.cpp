#include "ChannelMonitor.h"
#include "Configuration.h"

ChannelMonitor::ChannelMonitor(milliseconds a_periodDuration):
ChannelMonitorBase(a_periodDuration, Configuration::Instance().GetNumberOfEventsToDrawAfter(), "Channel Timing Monitor", "Channel Rate", "Intervals Between pulse", "Pulsetiming", "TimingHist")
{}

