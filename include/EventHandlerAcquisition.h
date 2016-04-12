#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include "CAENDigitizer.h"
#include "EventHandler.h"
#include "TFile.h"
#include "TTree.h"
#include "Queue.h"
#include "SignalAnalyzer.h"

using namespace std;

class EventHandlerAcquisition: public EventHandler
{
public:
	EventHandlerAcquisition(std::string a_sRootFileName);
	virtual ~EventHandlerAcquisition();
	void Handle(CAEN_DGTZ_X742_EVENT_t* a_pEvent, nanoseconds a_eventTime);
	void DoHandle(nanoseconds a_eventTime);
	void Stop();
	void ProcessEvents();

private:
	void PerformIntermediateAnalysis(nanoseconds a_eventTime);
	static void MainAnalysisThreadFunc(EventHandlerAcquisition* a_pEventHandler);

private:
	std::unique_ptr<TFile> m_pRootFile;
	TTree* m_pRootTree;
	unsigned int m_iNowMSB;
	unsigned int m_iNowLSB;
	bool m_bEventAddrSet;
	bool m_bEventInfoSet;
	std::unique_ptr<SignalAnalyzer> m_pSignalAnalyzer;
	int m_iEventCounter;
};

