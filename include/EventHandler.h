#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include "CAENDigitizer.h"
#include "TFile.h"
#include "TTree.h"
#include "Queue.h"
#include "SignalAnalyzer.h"

class EventHandler
{
public:
	EventHandler(/*int argc, char** argv*/std::string a_sRootOutFolder);
	~EventHandler();
	void Handle(CAEN_DGTZ_X742_EVENT_t* a_pEvent, time_point<high_resolution_clock> a_tp);
/*	void SetEventAddress();
	void SetEventInfoAddress();*/
	void Stop();

private:
	void PerformIntermediateAnalysis();
	void PrintEventInfo(CAEN_DGTZ_EventInfo_t* p_eventInfo);
//	void AssertReady();
	std::string GenerateFileName(std::string a_sRootOutFolder);
	static void MainAnalysisThreadFunc(EventHandler* a_pEventHandler);

private:
	std::unique_ptr<TFile> m_pRootFile;
	std::unique_ptr<TTree> m_pRootTree;
	//TODO: typedef this complex nested template
	std::vector<std::vector<float> > m_vChannels;
	time_point<high_resolution_clock> m_now;
	bool m_bEventAddrSet;
	bool m_bEventInfoSet;
	std::unique_ptr<SignalAnalyzer> m_pSignalAnalyzer;
};

