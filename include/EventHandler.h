#pragma once

#include <memory>
#include "CAENDigitizer.h"
#include "TFile.h"
#include "TTree.h"
/*
#ifdef __MAKECINT__
#pragma link C++ class std::vector<std::vector<float> >+;
#endif
*/
class EventHandler
{
public:
	EventHandler();
	~EventHandler();
	void Handle(CAEN_DGTZ_X742_EVENT_t* a_pEvent, CAEN_DGTZ_EventInfo_t* a_pEventInfo);
	std::string GenerateFileName();
	void SetEventAddress(CAEN_DGTZ_X742_EVENT_t* a_pEvent);
	void SetEventInfoAddress(CAEN_DGTZ_EventInfo_t* a_pEventInfo);
	void AssertReady();
private:
	std::unique_ptr<TFile> m_pRootFile;
	std::unique_ptr<TTree> m_pRootTree;
	std::vector<std::vector<float> > m_vChannels;
	bool m_bEventAddrSet;
	bool m_bEventInfoSet;
};

