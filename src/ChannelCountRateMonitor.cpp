#include "ChannelCountRateMonitor.h"
#include "Configuration.h"

ChannelCountRateMonitor::ChannelCountRateMonitor(milliseconds a_periodDuration):
ChannelCountRateMonitorBase(a_periodDuration, Configuration::Instance().GetNumberOfEventsToDrawAfter(), "Channel Count rate Monitor", "Channels Rate", "Intervals Between pulse", "Pulseratetiming", "TimingHistN")
{}

