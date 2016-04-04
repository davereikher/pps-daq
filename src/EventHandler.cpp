#include <iostream>
#include <tuple>
#include "EventHandler.h"
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

EventHandler::EventHandler(std::string a_sRootFileName):
m_pRootFile(new TFile(a_sRootFileName.c_str(), "RECREATE","", 3)),
m_pRootTree(new TTree(TREE_NAME, TREE_DESCRIPTION)),
m_bEventAddrSet(false),
m_bEventInfoSet(false),
m_pSignalAnalyzer(new SignalAnalyzer()),
m_iEventCounter(0)
{
	m_pRootTree->Branch("Event", &m_vChannels);
//	m_pRootTree->Branch("Empty", &m_bEventEmpty);
	m_pRootTree->Branch("ArrivalTimeMSB", &m_iNowMSB);
	m_pRootTree->Branch("ArrivalTimeLSB", &m_iNowLSB);

	m_pSignalAnalyzer->SetFlags(SignalAnalyzer::ETriggerTimingMonitor | SignalAnalyzer::EAsynchronous | SignalAnalyzer::EPanelHitMonitor);
	m_pSignalAnalyzer->Start();
}

/*void EventHandler::SetEventAddress()
{
	if(m_bEventAddrSet)
	{
		throw EventHandlerException(__LINE__, "Event address already set");
	}

	m_pRootTree->Branch("Event", &m_vChannels);
	m_bEventAddrSet = true;
}

void EventHandler::SetEventInfoAddress()
{
	if(m_bEventInfoSet)
	{
		throw EventHandlerException(__LINE__, "Event info already set");
	}
	//m_pRootTree->Branch("EventInfo", a_pEventInfo, "EventSize/i:BoardId:Pattern:ChannelMask:EventCounter:TriggerTimeTag");
//	m_pRootTree->Branch("EventInfo", &(a_pEventInfo->TriggerTimeTag), "TriggerTimeTag/i");
	m_pRootTree->Branch("EventInfo", &m_now);
	m_bEventInfoSet = true;
}
*/
/*void EventHandler::AssertReady()
{
	if (!(m_bEventAddrSet && m_bEventInfoSet))
	{
		throw EventHandlerException(__LINE__, "Not ready");
	}
}*/

void EventHandler::PrintEventInfo(CAEN_DGTZ_EventInfo_t* p_eventInfo)
{
	printf("Event %d info\n", p_eventInfo->EventCounter);
	printf("Event size: %d\n", p_eventInfo->EventSize);

	printf("Board id : %d\n", p_eventInfo->BoardId);
	printf("Pattern: %d\n", p_eventInfo->Pattern);
	printf("Channel mask: %08x\n", p_eventInfo->ChannelMask);
	printf("Triger time stamp: %u\n\n", p_eventInfo->TriggerTimeTag);
}

void EventHandler::Handle(CAEN_DGTZ_X742_EVENT_t* a_pEvent, nanoseconds a_eventTime)
{

	//PrintEventInfo(a_pEventInfo);
	
//	AssertReady();
	
	Logger::Instance().NewEntry(m_iEventCounter);
	m_iEventCounter ++;
	m_vChannels.clear();
	int iPresentGroup = -1;
	for (int iGroupCount = 0; iGroupCount < MAX_X742_GROUP_SIZE; iGroupCount++)
	{
		if(a_pEvent->GrPresent[iGroupCount])
		{
			if( iPresentGroup == -1)
			{
				iPresentGroup = iGroupCount;
			}

			//We loop until MAX_X742_CHANNEL_SIZE-1 because we're not interested in the group triggers, just the data on the channels.
			for (int iChannelCount = 0; iChannelCount < MAX_X742_CHANNEL_SIZE - 1; iChannelCount ++)  
			{
				uint32_t iChannelSize = a_pEvent->DataGroup[iGroupCount].ChSize[iChannelCount];
				float* pChannelPointer = a_pEvent->DataGroup[iGroupCount].DataChannel[iChannelCount];
				std::vector<float> vChannel(pChannelPointer, pChannelPointer + iChannelSize);
				m_vChannels.push_back(vChannel);
			}
		}
		else
		{
			for (int iChannelCount = 0; iChannelCount < MAX_X742_CHANNEL_SIZE - 1; iChannelCount ++)  
			{
				//push empty samples vector for an inactive channel
				std::vector<float> vChannel;
				m_vChannels.push_back(vChannel);
			}
		}

	}
	//Add precision trigger from one of the groups (they're all the same) to the end of the channels buffer (as channel 32)
	if( iPresentGroup != -1 )
	{
		uint32_t iChannelSize = a_pEvent->DataGroup[iPresentGroup].ChSize[MAX_X742_CHANNEL_SIZE - 1];
		float* pChannelPointer = a_pEvent->DataGroup[iPresentGroup].DataChannel[MAX_X742_CHANNEL_SIZE - 1];
		std::vector<float> vChannel(pChannelPointer, pChannelPointer + iChannelSize);
		m_vChannels.push_back(vChannel);
	}
	else
	{
		std::vector<float> vChannel;
		m_vChannels.push_back(vChannel);
	}

	if (IsEventEmpty())
	{
		m_vChannels.clear();
		std::vector<float> v;
		v.push_back(0);
		m_vChannels.push_back(v);
	}


	PerformIntermediateAnalysis(a_eventTime);
	//printf("%ld, now\t", (duration_cast<milliseconds>(a_tp.time_since_epoch()).count()));
	m_iNowLSB = (unsigned int) ((duration_cast<nanoseconds>(a_eventTime).count()) & 0x00000000FFFFFFFF);
	//printf("%ld, nowLSB\t", m_iNowLSB);
//	m_iNowLSB = (unsigned int) (duration_cast<nanoseconds>(a_tp.time_since_epoch()).count());
	m_iNowMSB = (unsigned int) (((duration_cast<nanoseconds>(a_eventTime).count()) & 0xFFFFFFFF00000000) >> 32);
	//printf("%ld, nowMSB\n", m_iNowMSB);


	m_pRootTree->Fill();	
}

EventHandler::~EventHandler()
{
	Stop();
}

bool EventHandler::IsEventEmpty()
{
	float fThreshold = Configuration::Instance().GetDigitizerResolution() * (Configuration::Instance().GetIdleFluctuationsAmplitude()/(Configuration::Instance().GetVoltMax() - Configuration::Instance().GetVoltMin()));
//	printf("fThreshold: %f\n", fThreshold);

	bool bSomeSignalFound = false;
	int i = 0;
	for (auto& chan: m_vChannels)
	{
		if (i == m_vChannels.size() - 1)
		{
			//We've reached the trigger data
			break;
		}
		float fMaxValue = FLT_MIN;
		float fMinValue = FLT_MAX;
		for (auto& sample: chan)
		{
			if(sample < fMinValue)
			{
				fMinValue = sample;	
			}
			if(sample > fMaxValue)
			{
				fMaxValue = sample;
			}
			if ( (fMaxValue - fMinValue) > fThreshold )
			{
				bSomeSignalFound = true;
				break;
			}
			if(bSomeSignalFound)
			{
				break;
			}
		}
		i++;
	}
	return !bSomeSignalFound;
}

void EventHandler::PerformIntermediateAnalysis(nanoseconds a_eventTime)
{
	m_pSignalAnalyzer->Analyze(a_eventTime, m_vChannels);
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

void EventHandler::Stop()
{
	m_iEventCounter = 0;
	m_pRootTree->Write();
	m_pRootFile->Close();
//	printf("Stoppin\n");
	m_pSignalAnalyzer->Stop();
//	printf("After stoppin\n");
}

void EventHandler::ProcessEvents()
{
	m_pSignalAnalyzer->ProcessEvents();
}
