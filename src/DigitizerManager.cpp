#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include "CAENDigitizer.h"
#include "ProprietaryUtils.h"
#include "Exception.h"
#include "DigitizerManager.h"

#define UNIX_PATH_MAX    108
#define CAEN_USE_DIGITIZERS
#define IGNORE_DPP_DEPRECATED
#define BOARD_ADDRESS 0xAAAA0000
#define MAXNB 1 /* Number of connected boards */
#define SAMPLING_FREQUENCY CAEN_DGTZ_DRS4_2_5GHz

/*
void hexDump (char *desc, void *addr, int len) {
	int i;
	unsigned char buff[17];
	unsigned char *pc = (unsigned char*)addr;

	// Output description if given.
	if (desc != NULL)
		printf ("%s:\n", desc);

	if (len == 0) {
		printf("  ZERO LENGTH\n");
		return;
	}
	if (len < 0) {
		printf("  NEGATIVE LENGTH: %i\n",len);
		return;
	}

	// Process every byte in the data.
	for (i = 0; i < len; i++) {
		// Multiple of 16 means new line (with line offset).

		if ((i % 16) == 0) {
			// Just don't print ASCII for the zeroth line.
			if (i != 0)
				printf ("  %s\n", buff);

			// Output the offset.
			printf ("  %04x ", i);
		}

		// Now the hex code for the specific character.
		printf (" %02x", pc[i]);

		// And store a printable ASCII character for later.
		if ((pc[i] < 0x20) || (pc[i] > 0x7e))
			buff[i % 16] = '.';
		else
			buff[i % 16] = pc[i];
		buff[(i % 16) + 1] = '\0';
	}

	// Pad out last line if not exactly 16 characters.
	while ((i % 16) != 0) {
		printf ("   ");
		i++;
	}

	// And print the final ASCII bit.
	printf ("  %s\n", buff);
}
*/

/*
void PrintEvent(Event& event)
{
	printf("Event size in bytes: %d\n", event.m_iEventSizeBytes);
	printf("board id: %d\n", event.m_iBoardId);
	printf("pattern: %d\n", event.m_iPattern);
	printf("group mask : %08x\n", event.m_iGroupMask);
	printf("event counter: %d\n", event.m_iEventCounter);
	printf("event time tag: %d\n", event.m_iEventTimeTag);		
}*/
/*
*/
/**
  Initializes and configures the V1742 board and returns a handle.

  @return Handle to the board
 */

DigitizerManager::DigitizerManager(EventHandler& a_eventHandler):
m_iHandle(0),
m_pEvent(NULL),
m_eventHandler(a_eventHandler),
m_pBuffer(NULL)
{}

DigitizerManager::~DigitizerManager()
{
	if(m_pBuffer)
	{
		CAEN_DGTZ_FreeReadoutBuffer(&m_pBuffer);
	}
	if(m_iHandle)
	{
		CAEN_DGTZ_CloseDigitizer(m_iHandle);
	}
	if(m_pEvent)
	{
		CAEN_DGTZ_FreeEvent(m_iHandle, (void**)&m_pEvent);
	}
}
#define COMBINE1(X,Y) X##Y
#define COMBINE(X,Y) COMBINE1(X,Y)
#define ASSERT_SUCCESS(action, message) int COMBINE(err,__LINE__); if((COMBINE(err,__LINE__) = (action))!=CAEN_DGTZ_Success) { throw InitException(__LINE__, (message), COMBINE(err,__LINE__)); }

void DigitizerManager::PrintBoardInfo(CAEN_DGTZ_BoardInfo_t& a_boardInfo)
{
	printf("\nConnected to CAEN Digitizer Model %s\n", a_boardInfo.ModelName);
	printf("\tROC FPGA Release is %s\n", a_boardInfo.ROC_FirmwareRel);
	printf("\tAMC FPGA Release is %s\n", a_boardInfo.AMC_FirmwareRel);
}

void DigitizerManager::InitAndConfigure()
{
	CAEN_DGTZ_BoardInfo_t BoardInfo;

	ASSERT_SUCCESS(CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB,0,0,BOARD_ADDRESS,&m_iHandle), "Failed to open digitizer");
	
	ASSERT_SUCCESS(CAEN_DGTZ_GetInfo(m_iHandle, &BoardInfo), "Failed to get digitizer  info");

	PrintBoardInfo(BoardInfo);

	ASSERT_SUCCESS(CAEN_DGTZ_Reset(m_iHandle), "Failed to reset digitizer");
//	ret = CAEN_DGTZ_SetRecordLength(m_iHandle,4096);                                /* Set the lenght of each waveform (in samples) */
//	CAEN_DGTZ_GetRecordLength(m_iHandle, &dat);
//	printf("Record length: %d\n", dat);
//	dat = -777;
//	CAEN_DGTZ_GetPostTriggerSize(m_iHandle, &dat);
//	printf("Post trigger size CAEN: %d\n", dat);
//	dat = Proprietary1742Utils::GetPostTriggerSetting(m_iHandle);
//	printf("Post trigger size proprietary: %d\n", dat);
//	grpEnableMask = Proprietary1742Utils::GetGroupEnableMask(m_iHandle);
//	printf("Group enable mask before setting: %d.\n", grpEnableMask);
	ASSERT_SUCCESS(Proprietary1742Utils::SetGroupEnableMask(m_iHandle, 1, 1, 1, 1), "Failed to set group enable mask (through WriteRegister)");
//	ASSERT_SUCCESS(CAEN_DGTZ_SetChannelDCOffset(m_iHandle, 16, 0xAFFF), "Could not set group DC offset");
/*	for (int i = 0; i < MAX_X742_GROUP_SIZE; i++)
	{	
		ASSERT_SUCCESS(CAEN_DGTZ_SetGroupDCOffset(m_iHandle, i, 0xFFFF), "Could not set group DC offset");
	} 
*/

//	eventSize = Proprietary1742Utils::GetEventSize(m_iHandle);
//	printf("Event size: %d.\n", eventSize);
//	boardType = Proprietary1742Utils::GetBoardType(m_iHandle);
//	printf("Board type: %d.\n", boardType);
//	grpEnableMask = Proprietary1742Utils::GetGroupEnableMask(m_iHandle);
//	printf("Group enable mask after setting: %d.\n", grpEnableMask);
	ASSERT_SUCCESS(CAEN_DGTZ_LoadDRS4CorrectionData(m_iHandle, SAMPLING_FREQUENCY), "Failed to load DRS4 correction data");
	ASSERT_SUCCESS(CAEN_DGTZ_EnableDRS4Correction(m_iHandle), "Failed to enable DRS4 correction");

//	CAEN_DGTZ_DRS4Frequency_t samplingFreq;
//	CAEN_DGTZ_GetDRS4SamplingFrequency(m_iHandle, &samplingFreq);
//	printf("Sampling frequency before setting: %d.\n", samplingFreq);
	ASSERT_SUCCESS(Proprietary1742Utils::SetSamplingFrequency(m_iHandle, CAEN_DGTZ_DRS4_2_5GHz), "Failed to set sampling frequency");
//	CAEN_DGTZ_GetDRS4SamplingFrequency(m_iHandle, &samplingFreq);
//	printf("Sampling frequency after setting: %d.\n", samplingFreq);

	ASSERT_SUCCESS(CAEN_DGTZ_SetExtTriggerInputMode(m_iHandle, CAEN_DGTZ_TRGMODE_ACQ_ONLY), "Failed to set external trigger mode");
	ASSERT_SUCCESS(CAEN_DGTZ_SetIOLevel(m_iHandle, CAEN_DGTZ_IOLevel_NIM), "Failed to set IO level to NIM");	
	ASSERT_SUCCESS(CAEN_DGTZ_SetSWTriggerMode(m_iHandle,CAEN_DGTZ_TRGMODE_DISABLED), "Failed to set SW trigger mode to disabled")
	ASSERT_SUCCESS(CAEN_DGTZ_SetMaxNumEventsBLT(m_iHandle,3), "Failed to set max num of events to be transferred");                           
	ASSERT_SUCCESS(CAEN_DGTZ_SetAcquisitionMode(m_iHandle,CAEN_DGTZ_FIRST_TRG_CONTROLLED), "Failed to set acquisition mode");	
	
	uint32_t size = 0; //dummy variable. Not used.
	ASSERT_SUCCESS(CAEN_DGTZ_MallocReadoutBuffer(m_iHandle,&m_pBuffer,&size), "Failed to allocate readout buffer");
	ASSERT_SUCCESS(CAEN_DGTZ_AllocateEvent(m_iHandle, (void**)&m_pEvent), "Failed to allocate event");

	m_eventHandler.SetEventInfoAddress(&m_eventInfo);
	m_eventHandler.SetEventAddress(m_pEvent);
}

void DigitizerManager::Start()
{
	ASSERT_SUCCESS(CAEN_DGTZ_SWStartAcquisition(m_iHandle), "Failed to start acquiring samples");
}

int DigitizerManager::Acquire()
{
	char * evtptr = NULL;
	uint32_t bsize;
	uint32_t iNumEvents;
	
	ASSERT_SUCCESS(CAEN_DGTZ_ReadData(m_iHandle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, m_pBuffer,&bsize), "Failed to read data");

	ASSERT_SUCCESS(CAEN_DGTZ_GetNumEvents(m_iHandle,m_pBuffer,bsize,&iNumEvents), "Failed to get number of events");

	for (uint32_t i=0; i < iNumEvents; i++) {
		/* Get the Infos and pointer to the event */
		ASSERT_SUCCESS(CAEN_DGTZ_GetEventInfo(m_iHandle,m_pBuffer,bsize,i,&m_eventInfo,&evtptr), "Failed to get event info");
		ASSERT_SUCCESS(CAEN_DGTZ_DecodeEvent(m_iHandle, m_pBuffer, (void**)&m_pEvent), "Failed to decode event");
		m_eventHandler.Handle(m_pEvent, &m_eventInfo);
	}

	return iNumEvents;

}

