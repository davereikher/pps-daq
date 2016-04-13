#include <iostream>
#include <tuple>
#include <cfloat>
#include "EventHandler.h"
#include "Exception.h"
#include "linux/limits.h"
#include "TSystem.h"
#include "TInterpreter.h"
#include "time.h"
#include "Configuration.h"
#include "EventHandlerScope.h"
#include "TSystem.h"

EventHandlerScope::EventHandlerScope():
m_pRangePlotter(
	new RangePlotter(
		Configuration::Instance().GetSamplingFreqGHz(),
		Configuration::Instance().GetVoltMin(),
		Configuration::Instance().GetVoltMax(),
		Configuration::Instance().GetDigitizerResolution(), 
		"Scope")
		),
m_ranges(Configuration::Instance().GetRanges())
{}

EventHandlerScope::~EventHandlerScope()
{}

void EventHandlerScope::DoHandle(nanoseconds a_eventTime)
{
	Range_t ranges = Configuration::Instance().GetRanges();
	m_pRangePlotter->PlotRanges(m_vChannels, m_ranges, "Scope");
	gSystem->ProcessEvents();
}


/*
void EventHandler::MainAnalysisThreadFunc(EventHandler* a_pEventHandler)
{
	static int iEventNum = 0;

	//The constructor of TApplication causes a segmentation violation, so we instantiate it on the heap and not delete it at the end. This is bad, but not fatal.
	TApplication* pApplication = new TApplication("app",0, 0);

	RangePlotter plt(Configuration::GetSamplingFreqGHz(), Configuration::GetVoltMin(), Configuration::GetVoltMax(),
		Configuration::GetDigitizerResolution());
	
	Range_t ranges = Configuration::GetRanges();

	bool bDone = false;
	while(!bDone)
	{
//		printf("queue size: %d, ", a_pEventHandler->m_queue.size());
		auto pair = a_pEventHandler->m_queue.pop();
		int iTimeStamp = pair.first;

		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
//		plt.PlotRanges(pair.second, ranges, std::string("Event Time Stamp ") + std::to_string(iTimeStamp)); 
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
//		printf("duration: %d\n", duration);

		sigAnalyzer.FindOriginalPulseInChannelRange(pair.second, ranges["A"]);
		if(sigAnalyzer.GetAnalysisMarkers().m_vChannelsWithPulse.size() > 0)
		{
			printf("GOT pulse at event %d in channels:\n", iEventNum);
			iEventNum ++;
			for (auto& itrtr: sigAnalyzer.GetAnalysisMarkers().m_vChannelsWithPulse)
			{
				printf("\t%d\n", itrtr);
			}
		}
		//plt.AddAnalysisMarkers(0, sigAnalyzer.GetAnalysisMarkers());
		//open new block for lock_guard
		{
			std::lock_guard<std::mutex> lockGuard(a_pEventHandler->m_mutex);
			bDone = a_pEventHandler->m_bStopAnalysisThread;
		}		
		gSystem->ProcessEvents();
	}
}
*/

