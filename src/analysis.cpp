#include <iostream>
#include "keyb.h"
#include "TFile.h"
#include "TTree.h"
#include <unistd.h>
#include "TApplication.h"
#include "SignalAnalyzer.h"
#include "CommonUtils.h"
#include "Configuration.h"
#include "Types.h"
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
	std::cout << "Usage: " << std::endl << "\t " << a_pProcName << " <path to configuration file> <path to root file> <path to log file>" << std::endl;
	std::vector<float> vec = CommonUtils::GenerateTimeSequence(5);
}

void ShowProgress(float progress)
{
	static int lastProgress = -1;
	if((int)progress == lastProgress)
	{
		return;
	}

	lastProgress =(int)progress;
	int barWidth = 70;

	std::cout << "[";
	int pos = barWidth * progress;
	for (int i = 0; i < barWidth; ++i) 
	{
		if (i < pos) 
		{
			std::cout << "=";
		}
		else if (i == pos) 
		{
			std::cout << ">";
		}
		else 
		{
			std::cout << " ";
		}
	}
	std::cout << "] " << int(progress * 100.0) << " %\r";
	std::cout.flush();
//	std::cout << std::endl;
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		Usage(argv[0]);
		exit(-1);
	}

	Configuration::Instance().LoadConfiguration(argv[1]);

	std::string sRootFileName(argv[2]);
	
	Logger::Instance().Init(argv[3], sRootFileName);
	Logger::Instance().AddNecessaryMessage(Configuration::Instance().GetDump());

	SignalAnalyzer sigAnalyzer;

	sigAnalyzer.SetFlags(SignalAnalyzer::ETriggerTimingSupervisor | SignalAnalyzer::EPanelSupervisor);

	sigAnalyzer.Start();
	
	Range_t mRanges = Configuration::Instance().GetRanges();

	Channels_t* vChannels = 0;
	unsigned int iArrivalTimeMSB = 0;
	unsigned int iArrivalTimeLSB = 0;

	TFile f(sRootFileName.c_str());
	TTree* tree = (TTree*)f.Get("DigitizerEvents");
	tree->SetBranchAddress("Event", &vChannels);
	tree->SetBranchAddress("ArrivalTimeMSB", &iArrivalTimeMSB);
	tree->SetBranchAddress("ArrivalTimeLSB", &iArrivalTimeLSB);


	int iNumOfEntries = tree->GetEntries();
	int c = 0;
	for (int i = 0; i < iNumOfEntries; i ++)
	{
		Logger::Instance().NewEntry(i);
		//ShowProgress(i/((float)iNumOfEntries));
		
		tree->GetEntry(i);

		int64_t iTimeStampNano = (((uint64_t)iArrivalTimeMSB) << 32) | iArrivalTimeLSB;
		//printf("MSB: %u, LSB: %u, time stamp: %llu\n", iArrivalTimeMSB, iArrivalTimeLSB, iTimeStampNano);
		sigAnalyzer.Analyze(nanoseconds(iTimeStampNano), *vChannels);
		c=getch();
		c = checkCommand();
		if (c == 1) break;

		
//		sigAnalyzer.FindOriginalPulseInChannelRange(vChannels, it.second);
	}
	sigAnalyzer.Flush();
	c = 0;

	printf("press q again to quit...\n");
	do
	{
		c = getch();
	} while(c != 'q');

	sigAnalyzer.Stop();
	return 0;
}
