#include <iostream>
#include <stdint.h>
#include "DigitizerManager.h"
#include "EventHandlerScope.h"
#include "Exception.h"
#include "keyb.h"
#include "TApplication.h"
#include "Configuration.h"
#include "CommonUtils.h"

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
	std::cout << "Usage: " << std::endl << "\t " << a_pProcName << " <path to configuration file>" << std::endl;
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		Usage(argv[0]);
		exit(-1);
	}

	TApplication* pApplication = new TApplication("app",0, 0);

	try
	{
		int c = 0;
		Configuration::Instance().LoadConfiguration(argv[1]);
		
		EventHandlerScope eventHandler;
		DigitizerManager digitizerManager(eventHandler);
		digitizerManager.InitAndConfigure();
		digitizerManager.Start();
		int i = 0;
		while ( true)
		{
			try	
			{
				digitizerManager.Acquire();
				eventHandler.ProcessEvents();
			}
			catch (ExceptionBase& ex)
			{
				printf("%s\n", ex.What().c_str());
				sleep(1);
			}
			c = checkCommand();
			if (c == 1) break;
		}

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
