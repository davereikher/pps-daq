#include <iostream>
#include "EventHandler.h"
#include "Exception.h"
#include "linux/limits.h"
#include "TSystem.h"
#include "TInterpreter.h"
#include "time.h"


#define TREE_NAME "DigitizerEvents"
#define TREE_DESCRIPTION "Events from V1742 CAEN digitizer"
#define FILE_NAME_FORMAT "pps-events-%d.root"
#define TOTAL_NUM_OF_CHANNELS MAX_X742_GROUP_SIZE * (MAX_X742_CHANNEL_SIZE - 1)

EventHandler::EventHandler(int argc, char** argv):
m_pRootFile(new TFile(GenerateFileName().c_str(), "RECREATE","", 3)),
m_pRootTree(new TTree(TREE_NAME, TREE_DESCRIPTION)),
m_bEventAddrSet(false),
m_bEventInfoSet(false),
m_plotter(argc, argv)
{}

void EventHandler::SetEventAddress(CAEN_DGTZ_X742_EVENT_t* a_pEvent)
{
	if(m_bEventAddrSet)
	{
		throw EventHandlerException(__LINE__, "Event address already set");
	}

	m_pRootTree->Branch("Event", &m_vChannels);
	m_bEventAddrSet = true;
}

void EventHandler::SetEventInfoAddress(CAEN_DGTZ_EventInfo_t* a_pEventInfo)
{
	if(m_bEventInfoSet)
	{
		throw EventHandlerException(__LINE__, "Event info already set");
	}
	//m_pRootTree->Branch("EventInfo", a_pEventInfo, "EventSize/i:BoardId:Pattern:ChannelMask:EventCounter:TriggerTimeTag");
	m_pRootTree->Branch("EventInfo", &(a_pEventInfo->TriggerTimeTag), "TriggerTimeTag/i");
	m_bEventInfoSet = true;
}


void EventHandler::AssertReady()
{
	if (!(m_bEventAddrSet && m_bEventInfoSet))
	{
		throw EventHandlerException(__LINE__, "Not ready");
	}
}

void EventHandler::PrintEventInfo(CAEN_DGTZ_EventInfo_t* p_eventInfo)
{
	printf("Event %d info\n", p_eventInfo->EventCounter);
	printf("Event size: %d\n", p_eventInfo->EventSize);

	printf("Board id : %d\n", p_eventInfo->BoardId);
	printf("Pattern: %d\n", p_eventInfo->Pattern);
	printf("Channel mask: %08x\n", p_eventInfo->ChannelMask);
	printf("Triger time stamp: %u\n\n", p_eventInfo->TriggerTimeTag);
}

void EventHandler::Handle(CAEN_DGTZ_X742_EVENT_t* a_pEvent, CAEN_DGTZ_EventInfo_t* a_pEventInfo)
{

/*	PrintEventInfo(a_pEventInfo);
	m_plotter.Plot(a_pEvent);*/
	
	AssertReady();
	
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

/*	std::vector<float> vec;
	vec.push_back((float)time(0));
	vec.push_back((float)time(0) /100);
	m_vChannels.clear();
	m_vChannels.push_back(vec);*/
	m_pRootTree->Fill();
	
//	m_pRootTree->Write();
}

std::string EventHandler::GenerateFileName()
{
	char sFileName [NAME_MAX];
	if (snprintf(sFileName, NAME_MAX, FILE_NAME_FORMAT, (int)time(0)) <= 0)
	{
		throw EventHandlerException(__LINE__, "TFile name generation failed");
	}

	//ROOT_FILE_OUT_DIR is defined in Makefile	
	std::string sStr = ROOT_FILE_OUT_DIR;
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
