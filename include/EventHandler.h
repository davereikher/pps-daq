#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include "CAENDigitizer.h"
#include "TFile.h"
#include "TTree.h"
#include "Queue.h"
#include "SignalAnalyzer.h"

using namespace std;

class EventHandler
{
public:
	EventHandler(std::string a_sRootFileName);
	~EventHandler();
	void Handle(CAEN_DGTZ_X742_EVENT_t* a_pEvent, nanoseconds a_eventTime);
	void Stop();
	void ProcessEvents();

private:
	void PerformIntermediateAnalysis(nanoseconds a_eventTime);
	void PrintEventInfo(CAEN_DGTZ_EventInfo_t* p_eventInfo);
	static void MainAnalysisThreadFunc(EventHandler* a_pEventHandler);
	bool IsEventEmpty();

private:
	std::unique_ptr<TFile> m_pRootFile;
	TTree* m_pRootTree;
	//TODO: typedef this complex nested template
	std::vector<std::vector<float> > m_vChannels;
	std::vector<float> m_vPrecisionTrigger;
	unsigned int m_iNowMSB;
	unsigned int m_iNowLSB;
	bool m_bEventAddrSet;
	bool m_bEventInfoSet;
	std::unique_ptr<SignalAnalyzer> m_pSignalAnalyzer;
	int m_iEventCounter;
};

