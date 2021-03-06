#include <iostream>
#include "keyb.h"
#include "TFile.h"
#include "TTree.h"
#include <unistd.h>
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
	std::cout << "Usage: " << std::endl << "\t " << a_pProcName << " <path to configuration file> <path to in root file> <path to out root file> <min. number of primary pulses>" << std::endl;
	std::vector<float> vec = CommonUtils::GenerateTimeSequence(5);
}

void ShowProgress(int a_iCurrentEntry, int a_iTotalEntries)
{
	static float lastPos = -1;
//	printf("progress: %d, lastProgress: %d\n", progress, lastProgress);

	float progress = ((float)a_iCurrentEntry)/a_iTotalEntries;
	int barWidth = 70;
	int pos = barWidth * progress;

	if(lastPos == pos)
	{
		return;
	}
	lastPos = pos;

	std::cout << "[";
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
	std::string sRootDestFileName(argv[3]);
	
/*	Logger::Instance().Init(argv[3]);
	Logger::Instance().AddNecessaryMessage(Configuration::Instance().GetDump());*/

	SignalAnalyzer sigAnalyzer;

	sigAnalyzer.SetFlags(SignalAnalyzer::AnalysisFlags::ECountPanelsWithPrimaryPulse);

	sigAnalyzer.Start();
	
	Range_t mRanges = Configuration::Instance().GetRanges();

	Channels_t* vChannels = 0;
	unsigned int iArrivalTimeMSB = 0;
	unsigned int iArrivalTimeLSB = 0;

	TFile f(sRootFileName.c_str());
	TTree* tree = (TTree*)f.Get("DigitizerEvents");
	TFile fOut(sRootDestFileName.c_str(), "RECREATE");
	TTree* outTree = tree->CloneTree(0);

	tree->SetBranchAddress("Event", &vChannels);
	tree->SetBranchAddress("ArrivalTimeMSB", &iArrivalTimeMSB);
	tree->SetBranchAddress("ArrivalTimeLSB", &iArrivalTimeLSB);

	

	int iNumOfEntries = tree->GetEntries();
	int c = 0;
	for (int i = 0; i < iNumOfEntries; i ++)
	{
		Logger::Instance().NewEntry(i);
		ShowProgress(i, iNumOfEntries);
		
		tree->GetEntry(i);

		int64_t iTimeStampNano = (((uint64_t)iArrivalTimeMSB) << 32) | iArrivalTimeLSB;
		//printf("MSB: %u, LSB: %u, time stamp: %llu\n", iArrivalTimeMSB, iArrivalTimeLSB, iTimeStampNano);
		sigAnalyzer.Analyze(nanoseconds(iTimeStampNano), *vChannels);
		if(sigAnalyzer.GetLastNumberOfPanelsWithPrimaryPulse() >= std::stoi(argv[4]))
		{
			outTree->Fill();
		}
//		c=getch();
		c = checkCommand();
		if (c == 1) break;

		
//		sigAnalyzer.FindOriginalPulseInChannelRange(vChannels, it.second);
	}
	sigAnalyzer.Flush();
	c = 0;

	printf("press q again to quit...\n");
	do
	{
		c = checkCommand();
		sigAnalyzer.ProcessEvents();
	} while(c != 1);

	sigAnalyzer.Stop();

	outTree->Write();
	delete outTree;
	delete tree;

	return 0;
}
