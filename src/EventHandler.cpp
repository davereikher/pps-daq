#include <iostream>
#include <tuple>
#include <chrono>
#include "EventHandler.h"
#include "Exception.h"
#include "linux/limits.h"
#include "TSystem.h"
#include "TInterpreter.h"
#include "time.h"
#include "SignalAnalyzer.h"
#include "RangePlotter.h"
#include "Configuration.h"
#include "TApplication.h"

using namespace std::chrono;

#define TREE_NAME "DigitizerEvents"
#define TREE_DESCRIPTION "Events from V1742 CAEN digitizer"
#define FILE_NAME_FORMAT "pps-events-%d.root"
#define TOTAL_NUM_OF_CHANNELS MAX_X742_GROUP_SIZE * (MAX_X742_CHANNEL_SIZE - 1)

EventHandler::EventHandler(std::string a_sRootOutFolder):
m_pRootFile(new TFile(GenerateFileName(a_sRootOutFolder).c_str(), "RECREATE","", 3)),
m_pRootTree(new TTree(TREE_NAME, TREE_DESCRIPTION)),
m_bEventAddrSet(false),
m_bEventInfoSet(false),
m_pSignalAnalyzer(new SignalAnalyzer(Configuration::GetSamplingFreqGHz(), Configuration::GetVoltMin(), 
		Configuration::GetVoltMax(), Configuration::GetDigitizerResolution(), Configuration::GetPulseThresholdVolts(), 
		Configuration::GetEdgeThresholdVolts(), Configuration::GetExpectedPulseWidthNs(), 
		Configuration::GetMinEdgeSeparationNs(), Configuration::GetMaxEdgeJitterNs(), 
		Configuration::GetMaxAmplitudeJitterVolts()))
{
	m_pRootTree->Branch("Event", &m_vChannels);
	m_pRootTree->Branch("ArrivalTime", &m_now);

	m_pSignalAnalyzer->SetFlags(SignalAnalyzer::ETriggerTimingSupervisor);
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

void EventHandler::Handle(CAEN_DGTZ_X742_EVENT_t* a_pEvent, time_point<high_resolution_clock> a_tp)
{

	//PrintEventInfo(a_pEventInfo);
	
//	AssertReady();
	
	m_vChannels.clear();
	for (int iGroupCount = 0; iGroupCount < MAX_X742_GROUP_SIZE; iGroupCount++)
	{
		if(a_pEvent->GrPresent[iGroupCount])
		{
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

	PerformIntermediateAnalysis();
	m_now = a_tp;

	m_pRootTree->Fill();	
}

std::string EventHandler::GenerateFileName(std::string a_sRootOutFolder)
{
	char sFileName [NAME_MAX];
	if (snprintf(sFileName, NAME_MAX, FILE_NAME_FORMAT, (int)time(0)) <= 0)
	{
		throw EventHandlerException(__LINE__, "TFile name generation failed");
	}

	std::string sStr = a_sRootOutFolder;

	if(sStr[sStr.size() - 1] != '/')
	{
		sStr += "/";
	}

	sStr += sFileName;

	std::cout << "Acquired data is stored in " << sStr << std::endl;
	
	return sStr;
}

EventHandler::~EventHandler()
{
	m_pRootTree->Write();
}

void EventHandler::PerformIntermediateAnalysis()
{
	m_pSignalAnalyzer->Analyze(high_resolution_clock::now(), m_vChannels);
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
	m_pRootTree->Write();
	m_pSignalAnalyzer->Stop();
}
