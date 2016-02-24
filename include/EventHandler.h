#pragma once

#include <memory>
#include "CAENDigitizer.h"
#include "TFile.h"
#include "TTree.h"
#include "Plotter.h"

class EventHandler
{
public:
	EventHandler(int argc, char** argv);
	~EventHandler();
	void Handle(CAEN_DGTZ_X742_EVENT_t* a_pEvent, CAEN_DGTZ_EventInfo_t* a_pEventInfo);
	void SetEventAddress(CAEN_DGTZ_X742_EVENT_t* a_pEvent);
	void SetEventInfoAddress(CAEN_DGTZ_EventInfo_t* a_pEventInfo);

private:
	void PerformIntermediateAnalysis();
	void PrintEventInfo(CAEN_DGTZ_EventInfo_t* p_eventInfo);
	void AssertReady();
	std::string GenerateFileName();

private:
	std::unique_ptr<TFile> m_pRootFile;
	std::unique_ptr<TTree> m_pRootTree;
	std::vector<std::vector<float> > m_vChannels;
	bool m_bEventAddrSet;
	bool m_bEventInfoSet;
	Plotter m_plotter;
};

