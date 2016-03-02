#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include "CAENDigitizer.h"
#include "TFile.h"
#include "TTree.h"
//#include "Plotter.h"
#include "Queue.h"

class EventHandler
{
public:
	EventHandler(/*int argc, char** argv*/std::string a_sRootOutFolder);
	~EventHandler();
	void Handle(CAEN_DGTZ_X742_EVENT_t* a_pEvent, CAEN_DGTZ_EventInfo_t* a_pEventInfo);
	void SetEventAddress(CAEN_DGTZ_X742_EVENT_t* a_pEvent);
	void SetEventInfoAddress(CAEN_DGTZ_EventInfo_t* a_pEventInfo);
	void Stop();

private:
	void PerformIntermediateAnalysis(CAEN_DGTZ_EventInfo_t* a_pEventInfo);
	void PrintEventInfo(CAEN_DGTZ_EventInfo_t* p_eventInfo);
	void AssertReady();
	std::string GenerateFileName(std::string a_sRootOutFolder);
	static void MainAnalysisThreadFunc(EventHandler* a_pEventHandler);

private:
	std::unique_ptr<TFile> m_pRootFile;
	std::unique_ptr<TTree> m_pRootTree;
	//TODO: typedef this complex nested template
	std::vector<std::vector<float> > m_vChannels;
	bool m_bEventAddrSet;
	bool m_bEventInfoSet;
	//Plotter m_plotter;
	std::thread m_analysisThread;
	Queue<std::pair<int, std::vector<std::vector<float> > > > m_queue;
	std::mutex m_mutex;
	bool m_bStopAnalysisThread;
};

