#include <iostream>
#include "DigitizerManager.h"
#include "EventHandler.h"
#include "Exception.h"
#include "keyb.h"
#include "TApplication.h"

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

int main(int argc, char** argv)
{
	TApplication m_pApplication("app",&argc,argv);

	try
	{
		int c = 0;
		EventHandler eventHandler(argc, argv);
		DigitizerManager digitizerManager(eventHandler);
		digitizerManager.InitAndConfigure();
		digitizerManager.Start();

		while (true)
		{
			digitizerManager.Acquire();
			c = checkCommand();
			if (c == 1) break;
		}
	}
	catch(ExceptionBase& ex)
	{
		std::cout << "CAUGHT EXCEPTION. Details:\t" << ex.What() << std::endl <<"Exiting..." << std::endl;
		return -1;
	}

	return 0;
}
