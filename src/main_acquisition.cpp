#include <iostream>
#include <stdint.h>
#include "DigitizerManager.h"
#include "EventHandlerAcquisition.h"
#include "Exception.h"
#include "keyb.h"
#include "TApplication.h"
#include "Configuration.h"
#include "Logger.h"
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
	std::cout << "Usage: " << std::endl << "\t " << a_pProcName << " <path to configuration file> <path to out folder> <description (surrounded by quotes)>" << std::endl;
}


std::pair<std::string, std::string> GenerateFiles(std::string a_sPath)
{
	if(a_sPath[a_sPath.size() - 1] != '/')
	{
		a_sPath += '/';
	}

	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	
	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer,80,"%Y-%m-%d-%I-%M-%S",timeinfo);

	a_sPath += buffer;
	
	if (CommonUtils::MkPath(a_sPath.c_str(), 0755) != 0)
	{
		printf("Failed to generate out path\n");
		exit(1);
	}
	
	std::string sLogFile = a_sPath + "/log";
	std::string sRootFile = a_sPath + "/data.root";

	return std::make_pair(sLogFile, sRootFile);
}

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		Usage(argv[0]);
		exit(-1);
	}

	try
	{
		std::pair<std::string, std::string> fileNames = GenerateFiles(argv[2]);
		int c = 0;
		Configuration::Instance().LoadConfiguration(argv[1]);
		Logger::Instance().Init(fileNames.first);
		Logger::Instance().AddNecessaryMessage(Configuration::Instance().GetDump());
		Logger::Instance().AddNecessaryMessage((std::string("\n\nSetup Description:\n---------------------\n ") + argv[3] + "\n\n").c_str());
		EventHandlerAcquisition eventHandler(fileNames.second);
		DigitizerManager digitizerManager(eventHandler);
		digitizerManager.InitAndConfigure();
		digitizerManager.Start();
		int i = 0;
		while ( true)
		{
			try	
			{
				digitizerManager.Acquire();
			}
			catch (ExceptionBase& ex)
			{
				printf("%s\n", ex.What().c_str());
				Logger::Instance().AddNecessaryMessage(ex.What() + "\n");
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
