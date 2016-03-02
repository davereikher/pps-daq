#include <iostream>
#include "DigitizerManager.h"
#include "EventHandler.h"
#include "Exception.h"
#include "keyb.h"
#include "TApplication.h"
#include "Configuration.h"

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
	std::cout << "Usage: " << std::endl << "\t " << a_pProcName << " <path to configuration file> <path to folder where output root file will be saved>" << std::endl;
}


int main(int argc, char** argv)
{
	//The constructor of TApplication causes a segmentation violation, so we instantiate it on the heap and not delete it at the end. This is bad, but not fatal.
//	TApplication* pApplication = new TApplication("app",&argc,argv);
	
	if (argc < 2)
	{
		Usage(argv[0]);
		exit(-1);
	}

	try
	{
		int c = 0;
		Configuration::LoadConfiguration(argv[1]);
		EventHandler eventHandler(argv[2]);
		DigitizerManager digitizerManager(eventHandler);
		digitizerManager.InitAndConfigure();
		digitizerManager.Start();
		int i = 0;
		while (/*i < 10*/ true)
		{
			if(digitizerManager.Acquire() > 0)
			{
//				i++;
			}
			c = checkCommand();
			if (c == 1) break;
		}
		eventHandler.Stop();
	}
	catch(ExceptionBase& ex)
	{
		std::cout << "CAUGHT EXCEPTION. Details:\t" << ex.What() << std::endl <<"Exiting..." << std::endl;
		return -1;
	}

	return 0;
}
