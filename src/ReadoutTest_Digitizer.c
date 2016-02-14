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
#include "Event.h"
#include "Exception.h"
#include "Plotter.h"
#include "EventHandler.h"
#include "TApplication.h"

#include "keyb.h"
#define UNIX_PATH_MAX    108
#define CAEN_USE_DIGITIZERS
#define IGNORE_DPP_DEPRECATED

#define MAXNB 1 /* Number of connected boards */

void LogEvent(char* eventptr, int size)
{
FILE *write_ptr;
char name[256];

snprintf(name, 256, "/tmp/vme/%ld-%ld", time(0), random());
write_ptr = fopen(name,"wb");  

fwrite(eventptr,size,1,write_ptr);
fclose(write_ptr);
}

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


void PrintEventInfo(CAEN_DGTZ_EventInfo_t* eventInfo)
{
	printf("Event %d info\n", eventInfo->EventCounter);
	printf("Event size: %d\n", eventInfo->EventSize);

	printf("Board id : %d\n", eventInfo->BoardId);
	printf("Pattern: %d\n", eventInfo->Pattern);
	printf("Channel mask: %08x\n", eventInfo->ChannelMask);
	printf("Triger time stamp: %d\n\n", eventInfo->TriggerTimeTag);
}

void PrintEvent(Event& event)
{
	printf("Event size in bytes: %d\n", event.m_iEventSizeBytes);
	printf("board id: %d\n", event.m_iBoardId);
	printf("pattern: %d\n", event.m_iPattern);
	printf("group mask : %08x\n", event.m_iGroupMask);
	printf("event counter: %d\n", event.m_iEventCounter);
	printf("event time tag: %d\n", event.m_iEventTimeTag);		
}

int checkCommand() {
	int c = 0;
	if(!kbhit())
			return 0;

	c = getch();
	switch (c) {
		case 's': 
			return 9;
			break;
		case 'k':
			return 1;
			break;
		case 'q':
			return 2;
			break;
	}
	return 0;
}

int DoMain()
{
	CAEN_DGTZ_ErrorCode ret;
	Plotter plt;

    /* The following variable will be used to get an handler for the digitizer. The
    handler will be used for most of CAENDigitizer functions to identify the board */
//	int	handle[MAXNB];
	int iHandle;
	CAEN_DGTZ_BoardInfo_t BoardInfo;
	CAEN_DGTZ_EventInfo_t eventInfo;
	//CAEN_DGTZ_UINT16_EVENT_t *Evt = NULL;
	char *buffer = NULL;

	int i;
	int c = 0, count = 0;
	char * evtptr = NULL;
	uint32_t size,bsize;
	uint32_t numEvents;
        CAEN_DGTZ_X742_EVENT_t* pEvent = NULL;
	unsigned int dat = -777;
	int grpEnableMask;
	int eventSize;
	int boardType;
	EventHandler eventHandler;
	eventHandler.SetEventInfoAddress(&eventInfo);

	i = sizeof(CAEN_DGTZ_TriggerMode_t);

	ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB,0,0,0xAAAA0000,&iHandle);
	if (ret != CAEN_DGTZ_Success) {
            printf("Can't open digitizer. ret = %d\n", ret);
            goto QuitProgram;
        }
        /* Once we have the handler to the digitizer, we use it to call the other functions */
        ret = CAEN_DGTZ_GetInfo(iHandle, &BoardInfo);
        printf("\nConnected to CAEN Digitizer Model %s", BoardInfo.ModelName);
        printf("\tROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
        printf("\tAMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);
	    
        ret = CAEN_DGTZ_Reset(iHandle);                                               /* Reset Digitizer */
	    ret = CAEN_DGTZ_GetInfo(iHandle, &BoardInfo);                                 /* Get Board Info */
	    ret = CAEN_DGTZ_SetRecordLength(iHandle,4096);                                /* Set the lenght of each waveform (in samples) */
//	    ret = CAEN_DGTZ_SetRecordLength(iHandle,1);                                /* Set the lenght of each waveform (in samples) */	    
	    CAEN_DGTZ_GetRecordLength(iHandle, &dat);
	    printf("Record length: %d\n", dat);
	    dat = -777;
	    CAEN_DGTZ_GetPostTriggerSize(iHandle, &dat);
	    printf("Post trigger size CAEN: %d\n", dat);
	    dat = Proprietary1742Utils::GetPostTriggerSetting(iHandle);
	    printf("Post trigger size proprietary: %d\n", dat);
	    grpEnableMask = Proprietary1742Utils::GetGroupEnableMask(iHandle);
	    printf("Group enable mask before setting: %d.\n", grpEnableMask);
//	    ret = CAEN_DGTZ_SetGroupEnableMask(iHandle,0x00000001);                              /* Enable channel 0 */
	    Proprietary1742Utils::SetGroupEnableMask(iHandle, 1, 1, 1, 1);
	    eventSize = Proprietary1742Utils::GetEventSize(iHandle);
	    printf("Event size: %d.\n", eventSize);
	    boardType = Proprietary1742Utils::GetBoardType(iHandle);
	    printf("Board type: %d.\n", boardType);
	    grpEnableMask = Proprietary1742Utils::GetGroupEnableMask(iHandle);
	    printf("Group enable mask after setting: %d.\n", grpEnableMask);
	    CAEN_DGTZ_LoadDRS4CorrectionData(iHandle, CAEN_DGTZ_DRS4_2_5GHz);
	    CAEN_DGTZ_EnableDRS4Correction(iHandle);
	    
	    CAEN_DGTZ_DRS4Frequency_t samplingFreq;
	    CAEN_DGTZ_GetDRS4SamplingFrequency(iHandle, &samplingFreq);
	    printf("Sampling frequency before setting: %d.\n", samplingFreq);
	    Proprietary1742Utils::SetSamplingFrequency(iHandle, CAEN_DGTZ_DRS4_2_5GHz);
	    CAEN_DGTZ_GetDRS4SamplingFrequency(iHandle, &samplingFreq);
	    printf("Sampling frequency after setting: %d.\n", samplingFreq);

//	    ret = CAEN_DGTZ_SetChannelTriggerThreshold(iHandle,0,32768);                  /* Set selfTrigger threshold */
//	    ret = CAEN_DGTZ_SetChannelSelfTrigger(iHandle,CAEN_DGTZ_TRGMODE_ACQ_ONLY,0xFFFFFFFF);  /* Set trigger on channel 0 to be ACQ_ONLY */
	    ret = CAEN_DGTZ_SetExtTriggerInputMode(iHandle, CAEN_DGTZ_TRGMODE_ACQ_ONLY); /* External trigger causes the board to only acquire data */
	    ret = CAEN_DGTZ_SetIOLevel(iHandle, CAEN_DGTZ_IOLevel_NIM);	/* Trigger is a NIM signal */
	    ret = CAEN_DGTZ_SetSWTriggerMode(iHandle,CAEN_DGTZ_TRGMODE_DISABLED);         /* Set the behaviour when a SW tirgger arrives */
	    ret = CAEN_DGTZ_SetMaxNumEventsBLT(iHandle,3);                                /* Set the max number of events to transfer in a sigle readout */
        ret = CAEN_DGTZ_SetAcquisitionMode(iHandle,CAEN_DGTZ_FIRST_TRG_CONTROLLED);          /* Set the acquisition mode */
        if(ret != CAEN_DGTZ_Success) {
            printf("Errors during Digitizer Configuration.\n");
            goto QuitProgram;
        }
	
        CAEN_DGTZ_AllocateEvent(iHandle, (void**)&pEvent);
	eventHandler.SetEventAddress(pEvent);
    //}
	printf("\n\nPress 's' to start the acquisition\n");
    printf("Press 'k' to stop  the acquisition\n");
    printf("Press 'q' to quit  the application\n\n");
    while (1) {
//		printf ("blah");
		c = checkCommand();
		if (c == 9) break;
		if (c == 2) return 0;
		Sleep(100);
    }
    ret = CAEN_DGTZ_MallocReadoutBuffer(iHandle,&buffer,&size);

    ret = CAEN_DGTZ_SWStartAcquisition(iHandle);

	while(1) {

		    ret = CAEN_DGTZ_ReadData(iHandle,CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer,&bsize); /* Read the buffer from the digitizer */

		    ret = CAEN_DGTZ_GetNumEvents(iHandle,buffer,bsize,&numEvents);
		
		    count +=numEvents;
		    for (i=0;i<(int)numEvents;i++) {
                /* Get the Infos and pointer to the event */
			    ret = CAEN_DGTZ_GetEventInfo(iHandle,buffer,bsize,i,&eventInfo,&evtptr);
	printf("USING CAEN_DGTZ_GetEventInfo:\n");
	PrintEventInfo(&eventInfo);
	printf("USING EVENT.CPP:\n");
        CAEN_DGTZ_DecodeEvent(iHandle, buffer, (void**)&pEvent);
	plt.Plot(pEvent);
	eventHandler.Handle(pEvent, &eventInfo);
	
	
		    }
		    c = checkCommand();
		    if (c == 1) goto Continue;
		    if (c == 2) goto Continue;
    } // end of readout loop

Continue:
	printf("\nRetrieved %d Events\n", count);
    goto QuitProgram;

/* Quit program routine */
QuitProgram:
    // Free the buffers and close the digitizers
	ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer);
        ret = CAEN_DGTZ_CloseDigitizer(iHandle);
	CAEN_DGTZ_FreeEvent(iHandle, (void**)&pEvent);
	printf("Press 'Enter' key to exit\n");
	c = getchar();
	return 0;
}

int main(int argc, char* argv[])
{
 TApplication app ("app",&argc,argv);
	//TSystem* gSystem = new TSystem();
	
//printf ("Creating canvas...\n");
//	   TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
//TH1D h ("h", "h", 10, 0, 10);
//h.Fill(1);
//h.Draw();	
//c1->Draw();
//	while(1);
//	
//   Double_t x[100], y[100];
  // Int_t n = 20;
   //for (Int_t i=0;i<n;i++) {
    // x[i] = i*0.1;
     //y[i] = 10*sin(x[i]+0.2);
  // }
   //TGraph * gr = new TGraph(n,x,y);
   //gr->Draw("AC*");	

	try
	{
		DoMain();
	}
	catch (EventParsingException& ex)
	{
		printf (ex.What().c_str());
		return -1;
	}

	return 0;
}
