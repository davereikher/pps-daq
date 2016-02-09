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

int OpenSocket()
{
  struct sockaddr_in address;
  struct hostent* server;
  int socket_fd, portno;
  socklen_t address_length;
  pid_t child;  

  char buffer[256];
  int n;
 portno = 5432;

 server = gethostbyname("127.0.0.1");
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0){
      printf("socket() failed: %d\n", errno);
      return 0;
  } 

 // unlink("/tmp/demo_socket");
  memset(&address, 0, sizeof(struct sockaddr_in));

  address.sin_family = AF_INET;
  address.sin_port = htons(portno);
bcopy((char *)server->h_addr, (char *)&address.sin_addr.s_addr, server->h_length);
//  snprintf(address.sin_path, UNIX_PATH_MAX, "/tmp/demo_socket");
/*
  if (bind(socket_fd, (struct sockaddr *) &address, sizeof(struct sockaddr_un)) != 0) {
      printf("bind() failed\n");
      return 1;
  }
  if(listen(socket_fd, 5) != 0) {
      printf("listen() failed\n");
      return 1;
  }*/

if (connect(socket_fd,(struct sockaddr*) &address,sizeof(address)) < 0)
{
  printf("ERROR connecting: %d", errno);
}

	return socket_fd;
}
void SendEvent(int socket_fd, char* event, int size)
{
	int err = 0;
	err = write(socket_fd, &size, sizeof(size));
	if (err < 0)
	{
		printf("ERROR writing size: %d", errno);
	}

	err = write(socket_fd, event, size);
	if (err < 0)
	{
		printf("ERROR writing event: %d", errno);
	}

/*  address_length = sizeof(address);  
  while((connection_fd = accept(socket_fd, 
                   (struct sockaddr *) &address,
                   &address_length)) > -1)
  {
      printf("successfully received data\n");
      bzero(buffer,256);
      n = read(connection_fd,buffer,255);
      if (n < 0) error("ERROR reading from socket");
         printf("Here is the message: %s\n\n", buffer);
      strcpy(buffer, "Hi back from the C world"); 
      n = write(connection_fd, buffer, strlen(buffer));
      if (n < 0) 
         printf("ERROR writing to socket\n");           
      break;      
  }
  close(socket_fd);
  close(socket_fd);
  return(0);*/
}

/*int main (int argc, char *argv[]) {
    char my_str[] = "a char string greater than 16 chars";
    hexDump ("my_str", &my_str, sizeof (my_str));
    return 0;
}*/

void PrintEventInfo(CAEN_DGTZ_EventInfo_t* eventInfo)
{
	printf("Event %d info\n", eventInfo->EventCounter);
	printf("Event size: %d\n", eventInfo->EventSize);

	printf("Board id : %d\n", eventInfo->BoardId);
	printf("Pattern: %d\n", eventInfo->Pattern);
	printf("Channel mask: %08x\n", eventInfo->ChannelMask);
	printf("Triger time stamp: %d\n\n", eventInfo->TriggerTimeTag);
}
void PrintEvent(CAEN_DGTZ_UINT16_EVENT_t *Evt)
{
	
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

void SendDummies()
{
	int i = 0;
	int sock_fd = OpenSocket();
 	printf("sock_fd: %d", sock_fd);	
	#define EVENT_SIZE 49200
	char dummy[EVENT_SIZE];
	FILE* file;
	int err = 0;
	file = fopen("/tmp/vme/1454598277-861109485", "rb");
	if (file == NULL)
	{
		printf("Error opening dummy file: %d", errno);
	}

	err = fread(dummy, EVENT_SIZE, 1, file);
	fclose(file);
	printf("Sending dummies...\n");
	for (i = 0; i < 1000; i++)
	{
  		/*memset(dummy, 0, EVENT_SIZE);
		snprintf(dummy, 15, "dummy%d", i);
		snprintf(dummy + EVENT_SIZE - 3, 3, "end");
*/
		SendEvent(sock_fd, dummy, EVENT_SIZE);
	}	
}

int main(int argc, char* argv[])
{
//	SendDummies();
//	return 0;
    /* The following variable is the type returned from most of CAENDigitizer
    library functions and is used to check if there was an error in function
    execution. For example:
    ret = CAEN_DGTZ_some_function(some_args);
    if(ret) printf("Some error"); */
	//while(1)
//	SendEvent();	
//exit(1);
CAEN_DGTZ_ErrorCode ret;

    /* The following variable will be used to get an handler for the digitizer. The
    handler will be used for most of CAENDigitizer functions to identify the board */
	int	handle[MAXNB];

    CAEN_DGTZ_BoardInfo_t BoardInfo;
	CAEN_DGTZ_EventInfo_t eventInfo;
	CAEN_DGTZ_UINT16_EVENT_t *Evt = NULL;
	char *buffer = NULL;

	int i,b;
	int c = 0, count[MAXNB];
	char * evtptr = NULL;
	uint32_t size,bsize;
	uint32_t numEvents;
	i = sizeof(CAEN_DGTZ_TriggerMode_t);

    for(b=0; b<MAXNB; b++){
        /* IMPORTANT: The following function identifies the different boards with a system which may change
        for different connection methods (USB, Conet, ecc). Refer to CAENDigitizer user manual for more info.
        brief:
            CAEN_DGTZ_OpenDigitizer(<LikType>, <LinkNum>, <ConetNode>, <VMEBaseAddress>, <*handler>);
        Some examples below */
        
        /* The following is for b boards connected via b USB direct links
        in this case you must set <LikType> = CAEN_DGTZ_USB and <ConetNode> = <VMEBaseAddress> = 0 */
        //ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB, b, 0, 0, &handle[b]);

        /* The following is for b boards connected via 1 opticalLink in dasy chain
        in this case you must set <LikType> = CAEN_DGTZ_PCI_OpticalLink and <LinkNum> = <VMEBaseAddress> = 0 */
        //ret = CAEN_DGTZ_OpenDigitizer(Params[b].LinkType, 0, b, Params[b].VMEBaseAddress, &handle[b]);

        /* The following is for b boards connected to A2818 (or A3818) via opticalLink (or USB with A1718)
        in this case the boards are accessed throught VME bus, and you must specify the VME address of each board:
        <LikType> = CAEN_DGTZ_PCI_OpticalLink (CAEN_DGTZ_PCIE_OpticalLink for A3818 or CAEN_DGTZ_USB for A1718)
        <LinkNum> must be the bridge identifier
        <ConetNode> must be the port identifier in case of A2818 or A3818 (or 0 in case of A1718)
        <VMEBaseAddress>[0] = <0xXXXXXXXX> (address of first board) 
        <VMEBaseAddress>[1] = <0xYYYYYYYY> (address of second board) 
        ...
        <VMEBaseAddress>[b-1] = <0xZZZZZZZZ> (address of last board)
        See the manual for details */
        ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB,0,0,0xAAAA0000,&handle[b]);
        if(ret != CAEN_DGTZ_Success) {
            printf("Can't open digitizer. ret = %d\n", ret);
            goto QuitProgram;
        }
        /* Once we have the handler to the digitizer, we use it to call the other functions */
        ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo);
        printf("\nConnected to CAEN Digitizer Model %s, recognized as board %d\n", BoardInfo.ModelName, b);
        printf("\tROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
        printf("\tAMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);
	    
        ret = CAEN_DGTZ_Reset(handle[b]);                                               /* Reset Digitizer */
	    ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo);                                 /* Get Board Info */
//	    ret = CAEN_DGTZ_SetRecordLength(handle[b],4096);                                /* Set the lenght of each waveform (in samples) */
	    ret = CAEN_DGTZ_SetRecordLength(handle[b],1);                                /* Set the lenght of each waveform (in samples) */	    
	    unsigned int dat = -777;
	    CAEN_DGTZ_GetRecordLength(handle[b], &dat);
	    printf("Record length: %d\n", dat);
	    dat = -777;
	    CAEN_DGTZ_GetPostTriggerSize(handle[b], &dat);
	    printf("Post trigger size CAEN: %d\n", dat);
	    dat = Proprietary1742Utils::GetPostTriggerSetting(handle[b]);
	    printf("Post trigger size proprietary: %d\n", dat);
	    int grpEnableMask = Proprietary1742Utils::GetGroupEnableMask(handle[b]);
	    printf("Group enable mask before setting: %d.\n", grpEnableMask);
//	    ret = CAEN_DGTZ_SetGroupEnableMask(handle[b],0x00000001);                              /* Enable channel 0 */
	    Proprietary1742Utils::SetGroupEnableMask(handle[b], 1, 1, 1, 1);
	    int eventSize = Proprietary1742Utils::GetEventSize(handle[b]);
	    printf("Event size: %d.\n", eventSize);
	    int boardType = Proprietary1742Utils::GetBoardType(handle[b]);
	    printf("Board type: %d.\n", boardType);
	    grpEnableMask = Proprietary1742Utils::GetGroupEnableMask(handle[b]);
	    printf("Group enable mask after setting: %d.\n", grpEnableMask);
	    
	    CAEN_DGTZ_DRS4Frequency_t samplingFreq;
	    CAEN_DGTZ_GetDRS4SamplingFrequency(handle[b], &samplingFreq);
	    printf("Sampling frequency before setting: %d.\n", samplingFreq);
	    Proprietary1742Utils::SetSamplingFrequency(handle[b], CAEN_DGTZ_DRS4_2_5GHz);
	    CAEN_DGTZ_GetDRS4SamplingFrequency(handle[b], &samplingFreq);
	    printf("Sampling frequency after setting: %d.\n", samplingFreq);

//	    ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle[b],0,32768);                  /* Set selfTrigger threshold */
//	    ret = CAEN_DGTZ_SetChannelSelfTrigger(handle[b],CAEN_DGTZ_TRGMODE_ACQ_ONLY,0xFFFFFFFF);  /* Set trigger on channel 0 to be ACQ_ONLY */
	    ret = CAEN_DGTZ_SetExtTriggerInputMode(handle[b], CAEN_DGTZ_TRGMODE_ACQ_ONLY); /* External trigger causes the board to only acquire data */
	    ret = CAEN_DGTZ_SetIOLevel(handle[b], CAEN_DGTZ_IOLevel_NIM);	/* Trigger is a NIM signal */
	    ret = CAEN_DGTZ_SetSWTriggerMode(handle[b],CAEN_DGTZ_TRGMODE_DISABLED);         /* Set the behaviour when a SW tirgger arrives */
	    ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle[b],3);                                /* Set the max number of events to transfer in a sigle readout */
        ret = CAEN_DGTZ_SetAcquisitionMode(handle[b],CAEN_DGTZ_FIRST_TRG_CONTROLLED);          /* Set the acquisition mode */
        if(ret != CAEN_DGTZ_Success) {
            printf("Errors during Digitizer Configuration.\n");
            goto QuitProgram;
        }
    }
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
//	printf ("%d\n", __LINE__);
    /* Malloc Readout Buffer.
    NOTE1: The mallocs must be done AFTER digitizer's configuration!
    NOTE2: In this example we use the same buffer, for every board. We
    Use the first board to allocate the buffer, so if the configuration
    is different for different boards (or you use different board models), may be
    that the size to allocate must be different for each one. */
    ret = CAEN_DGTZ_MallocReadoutBuffer(handle[0],&buffer,&size);

    
    for(b=0; b<MAXNB; b++)
//	printf("%d\n", __LINE__);
            /* Start Acquisition
            NB: the acquisition for each board starts when the following line is executed
            so in general the acquisition does NOT starts syncronously for different boards */
            ret = CAEN_DGTZ_SWStartAcquisition(handle[b]);

//	printf("%d\n", __LINE__);
            /* Start Acquisition*/
    // Start acquisition loop
	while(1) {
        for(b=0; b<MAXNB; b++) {
		    //ret = CAEN_DGTZ_SendSWtrigger(handle[b]); /* Send a SW Trigger */

		    ret = CAEN_DGTZ_ReadData(handle[b],CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer,&bsize); /* Read the buffer from the digitizer */

            /* The buffer red from the digitizer is used in the other functions to get the event data
            The following function returns the number of events in the buffer */
		    ret = CAEN_DGTZ_GetNumEvents(handle[b],buffer,bsize,&numEvents);
		


//	printf("%d. ret = %d. Num of events: %d\n", __LINE__, ret, numEvents);
//		    printf(".");

		    count[b] +=numEvents;
		    for (i=0;i<numEvents;i++) {
                /* Get the Infos and pointer to the event */
			    ret = CAEN_DGTZ_GetEventInfo(handle[b],buffer,bsize,i,&eventInfo,&evtptr);
	PrintEventInfo(&eventInfo);
	//printf("Logging event %d...\n",eventInfo.EventCounter); 
	//SendEvent(evtptr, eventInfo.EventSize);
	LogEvent(evtptr, eventInfo.EventSize);
	/*printf("++++++++++++++++++++");
	hexDump("event dump", evtptr, eventInfo.EventSize);
	printf("********************")*/
//	printf("%d. ret = %d\n", __LINE__, ret);

	

                /* Decode the event to get the data */
				//Evt = NULL;
//			    ret = CAEN_DGTZ_DecodeEvent(handle[b],evtptr,&Evt);
				


//	printf("%d. ret = %d\n", __LINE__, ret);
			    //*************************************
			    // Event Elaboration
			    //*************************************
		//	    ret = CAEN_DGTZ_FreeEvent(handle[b],&Evt);
		//		Evt = NULL;
//	PrintEvent(Evt);


//	printf("%d. ret = %d\n", __LINE__, ret);
		    }
		    c = checkCommand();
		    if (c == 1) goto Continue;
		    if (c == 2) goto Continue;
        } // end of loop on boards
    } // end of readout loop

Continue:
    for(b=0; b<MAXNB; b++)
        printf("\nBoard %d: Retrieved %d Events\n",b, count[b]);
    goto QuitProgram;

/* Quit program routine */
QuitProgram:
    // Free the buffers and close the digitizers
	ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer);
    for(b=0; b<MAXNB; b++)
        ret = CAEN_DGTZ_CloseDigitizer(handle[b]);
	printf("Press 'Enter' key to exit\n");
	c = getchar();
	return 0;
}

