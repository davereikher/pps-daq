#include <iostream>
#include <stdint.h>
#include "DigitizerManager.h"
#include "EventHandler.h"
#include "Exception.h"
#include "keyb.h"
#include "TApplication.h"
#include "Configuration.h"
#include "Logger.h"

int checkCommand() {
	int c = 0;
	if(!kbhit())
		return 0;

	c = getch();
	switch (c) {
		case 'q':
			return 1;
			break;
	}
	return 0;
}

void Usage(char* a_pProcName)
{
	std::cout << "Usage: " << std::endl << "\t " << a_pProcName << " <path to configuration file> <path to folder where output root file will be saved> <path to log file>" << std::endl;
}


int main(int argc, char** argv)
{
	//The constructor of TApplication causes a segmentation violation, so we instantiate it on the heap and not delete it at the end. This is bad, but not fatal.
//	TApplication* pApplication = new TApplication("app",&argc,argv);
	
	if (argc < 3)
	{
		Usage(argv[0]);
		exit(-1);
	}

	try
	{
		printf("size of int64: %d, %d", sizeof(uint64_t), sizeof(int64_t));
		int c = 0;
		Configuration::Instance().LoadConfiguration(argv[1]);
		//TODO: generate log file automatically. Fix the bug that root file path doesn't appear in log file by moving the log init into EventHandler and generating the log file name automatically, based on the root file name.
		Logger::Instance().Init(argv[3], argv[2]);
		Logger::Instance().AddNecessaryMessage(Configuration::Instance().GetDump());
		EventHandler eventHandler(argv[2]);
		DigitizerManager digitizerManager(eventHandler);
		digitizerManager.InitAndConfigure();
		digitizerManager.Start();
		int i = 0;
		while (/*i < 10*/ true)
		{
			if(digitizerManager.Acquire() > 0)
			{
		//		i++;
			}
			c = checkCommand();
			if (c == 1) break;
		}
		eventHandler.Stop();

		std::cout << "Press 'q' again to quit..."<< std::endl;
		fflush(stdout);

		do
		{
			c = checkCommand();
			eventHandler.ProcessEvents();
		}while (c != 1);
	}
	catch(ExceptionBase& ex)
	{
		std::cout << "CAUGHT EXCEPTION. Details:\t" << ex.What() << std::endl << "Exiting..." << std::endl;
		return -1;
	}

	return 0;
}
