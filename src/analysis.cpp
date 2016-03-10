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

void Usage(char* a_pProcName)
{
	std::cout << "Usage: " << std::endl << "\t " << a_pProcName << " <path to configuration file> <path to root file>" << std::endl;
	std::vector<float> vec = CommonUtils::GenerateTimeSequence(5);
}


int main(int argc, char* argv[])
{
	Configuration::LoadConfiguration(argv[1]);

	std::string sRootFileName(argv[2]);

	SignalAnalyzer sigAnalyzer;

	sigAnalyzer.SetFlags(SignalAnalyzer::ETriggerTimingSupervisor);

	sigAnalyzer.Start();

	if (argc < 2)
	{
		Usage(argv[0]);
		exit(-1);
	}
	
	Range_t mRanges = Configuration::GetRanges();

	Channels_t* vChannels = 0;
	unsigned int iArrivalTimeMSB = 0;
	unsigned int iArrivalTimeLSB = 0;

	TFile f(sRootFileName.c_str());
	TTree* tree = (TTree*)f.Get("DigitizerEvents");
	tree->SetBranchAddress("Event", &vChannels);
	tree->SetBranchAddress("ArrivalTimeMSB", &iArrivalTimeMSB);
	tree->SetBranchAddress("ArrivalTimeLSB", &iArrivalTimeLSB);


	int iNumOfEntries = tree->GetEntries();
	for (int i = 0; i < iNumOfEntries; i ++)
	{
		
		tree->GetEntry(i);

		int64_t iTimeStampNano = (((uint64_t)iArrivalTimeMSB) << 32) | iArrivalTimeLSB;
		printf("MSB: %u, LSB: %u, time stamp: %llu\n", iArrivalTimeMSB, iArrivalTimeLSB, iTimeStampNano);
		sigAnalyzer.Analyze(nanoseconds(iTimeStampNano), *vChannels);
//		sigAnalyzer.FindOriginalPulseInChannelRange(vChannels, it.second);
	}
	sigAnalyzer.Flush();
	int c = 0;

	printf("press q to quit...\n");
	do
	{
		c = getch();
	} while(c != 'q');

	sigAnalyzer.Stop();
	return 0;
}
