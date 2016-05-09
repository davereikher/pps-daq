#include <iostream>
#include <fstream>
#include "keyb.h"
#include "TFile.h"
#include "TTree.h"
#include <unistd.h>
#include "TApplication.h"
#include "SignalAnalyzer.h"
#include "CommonUtils.h"
#include "Configuration.h"
#include "TChain.h"
#include "Types.h"
#include "Logger.h"

#include "RandomTrackGenerator.h"
#include "TH1.h"
#include "SimulationEngine.h"

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

std::vector<std::string> GetVectorOfDataFileNames(std::string sListFile)
{	
	std::ifstream file;
	std::string line;
	std::vector<std::string> vResult;

	file.open(sListFile.c_str());
	if(file.is_open())
	{
		while(getline(file, line))
		{
	        	vResult.push_back(line);
		}
	}

	file.close();
	return vResult;
}

int main(int argc, char* argv[])
{
	TApplication* pApplication = new TApplication("app",0, 0);

	if (argc < 2)
	{
		Usage(argv[0]);
		exit(-1);
	}

	Configuration::Instance().LoadConfiguration(argv[1]);

	RandomTrackGenerator rg(Geometry::HorizontalRectangle3D(Configuration::Instance().GetMonteCarloPointDomainRectangleLengthXMm(),
			Configuration::Instance().GetMonteCarloPointDomainRectangleLengthYMm(), 
			Configuration::Instance().GetMonteCarloPointDomainRectangleCenterXMm(), 
			Configuration::Instance().GetMonteCarloPointDomainRectangleCenterYMm(),
			Configuration::Instance().GetMonteCarloPointDomainRectangleCenterZMm()));

/*	TCanvas pCanvas("test_distributions", "Test distributions", 800, 600);
	pCanvas.Divide(1,2);
	TH1F pPhiHist("Phi" , "Phi", 100, 0, 0);
	pPhiHist.SetFillColor(49);
	TH1F pThetaHist("Phi" , "Phi", 100, 0, 0);
	pThetaHist.SetFillColor(49);
*/	
	SimulationEngine simEngine(rg);
	simEngine.Draw();

	for (int i = 0; i < 100; i++)
	{
		simEngine.SingleRun();
	}

/*	pCanvas.cd(1);
	pPhiHist.Draw();
	pCanvas.cd(2);
	pThetaHist.Draw();
	pCanvas.WaitPrimitive();
*/
	
/*	if (argc < 3)
	{
		Usage(argv[0]);
		exit(-1);
	}

	Configuration::Instance().LoadConfiguration(argv[1]);

	std::string sRootFileName(argv[2]);
	
	Logger::Instance().Init(argv[3]);
	Logger::Instance().AddNecessaryMessage(Configuration::Instance().GetDump());

	SignalAnalyzer sigAnalyzer;

	sigAnalyzer.SetFlags(SignalAnalyzer::ETrackMonitor);

	sigAnalyzer.Start();
	
	Range_t mRanges = Configuration::Instance().GetRanges();

	Channels_t* vChannels = 0;
	unsigned int iArrivalTimeMSB = 0;
	unsigned int iArrivalTimeLSB = 0;

	std::vector<std::string> vDataFileNames = GetVectorOfDataFileNames(argv[2]);

//	TFile f(sRootFileName.c_str());
	TChain chain("DigitizerEvents");
	for (auto & it : vDataFileNames)
	{
		printf("file name : %s\n", it.c_str());
		chain.Add(it.c_str());
	}
//	TTree* tree = (TTree*)f.Get("DigitizerEvents");
	chain.SetBranchAddress("Event", &vChannels);
	chain.SetBranchAddress("ArrivalTimeMSB", &iArrivalTimeMSB);
	chain.SetBranchAddress("ArrivalTimeLSB", &iArrivalTimeLSB);


	int iNumOfEntries = chain.GetEntries();
	int c = 0;
	for (int i = 0; i < iNumOfEntries; i ++)
	{
		Logger::Instance().NewEntry(i);
		ShowProgress(i, iNumOfEntries);
		
		chain.GetEntry(i);

		int64_t iTimeStampNano = (((uint64_t)iArrivalTimeMSB) << 32) | iArrivalTimeLSB;
		//printf("MSB: %u, LSB: %u, time stamp: %llu\n", iArrivalTimeMSB, iArrivalTimeLSB, iTimeStampNano);
		sigAnalyzer.Analyze(nanoseconds(iTimeStampNano), *vChannels);
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

	sigAnalyzer.Stop();*/
	simEngine.Wait();
	return 0;
}
