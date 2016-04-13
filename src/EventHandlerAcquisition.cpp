#include <iostream>
#include <tuple>
#include "EventHandlerAcquisition.h"
#include "Exception.h"
#include "linux/limits.h"
#include "TSystem.h"
#include "TInterpreter.h"
#include "time.h"
#include "SignalAnalyzer.h"
#include "RangePlotter.h"
#include "Configuration.h"
#include "Logger.h"
#include "TApplication.h"
#include "TROOT.h"

#define TREE_NAME "DigitizerEvents"
#define TREE_DESCRIPTION "Events from V1742 CAEN digitizer"
#define FILE_NAME_FORMAT "pps-events-%d.root"
#define TOTAL_NUM_OF_CHANNELS MAX_X742_GROUP_SIZE * (MAX_X742_CHANNEL_SIZE - 1)

EventHandlerAcquisition::EventHandlerAcquisition(std::string a_sRootFileName):
m_pRootFile(new TFile(a_sRootFileName.c_str(), "RECREATE","", 3)),
m_pRootTree(new TTree(TREE_NAME, TREE_DESCRIPTION)),
m_bEventAddrSet(false),
m_bEventInfoSet(false),
m_pSignalAnalyzer(new SignalAnalyzer()),
m_iEventCounter(0)
{
	m_pRootTree->Branch("Event", &m_vChannels);
	m_pRootTree->Branch("ArrivalTimeMSB", &m_iNowMSB);
	m_pRootTree->Branch("ArrivalTimeLSB", &m_iNowLSB);

	m_pSignalAnalyzer->SetFlags(SignalAnalyzer::ETriggerTimingMonitor | SignalAnalyzer::EAsynchronous | SignalAnalyzer::EPanelHitMonitor);
	m_pSignalAnalyzer->Start();
}

void EventHandlerAcquisition::DoHandle(nanoseconds a_eventTime)
{

	//PrintEventInfo(a_pEventInfo);
	
//	AssertReady();
	
	Logger::Instance().NewEntry(m_iEventCounter);
	m_iEventCounter ++;

	PerformIntermediateAnalysis(a_eventTime);
	//printf("%ld, now\t", (duration_cast<milliseconds>(a_tp.time_since_epoch()).count()));
	m_iNowLSB = (unsigned int) ((duration_cast<nanoseconds>(a_eventTime).count()) & 0x00000000FFFFFFFF);
	//printf("%ld, nowLSB\t", m_iNowLSB);
//	m_iNowLSB = (unsigned int) (duration_cast<nanoseconds>(a_tp.time_since_epoch()).count());
	m_iNowMSB = (unsigned int) (((duration_cast<nanoseconds>(a_eventTime).count()) & 0xFFFFFFFF00000000) >> 32);
	//printf("%ld, nowMSB\n", m_iNowMSB);


	m_pRootTree->Fill();	
}

EventHandlerAcquisition::~EventHandlerAcquisition()
{
	Stop();
}

void EventHandlerAcquisition::PerformIntermediateAnalysis(nanoseconds a_eventTime)
{
	m_pSignalAnalyzer->Analyze(a_eventTime, m_vChannels);
}

void EventHandlerAcquisition::Stop()
{
	m_iEventCounter = 0;
	m_pRootTree->Write();
	m_pRootFile->Close();
	m_pSignalAnalyzer->Stop();
}

void EventHandlerAcquisition::ProcessEvents()
{
	m_pSignalAnalyzer->ProcessEvents();
}

