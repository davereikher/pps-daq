#include <iostream>
#include <fstream>
#include <ctime>
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
#include "TROOT.h"
#include "TCanvas.h"
#include "TCollection.h"

int checkCommand() {
	int c = 0;
	if(!kbhit())
		return 0;

	c = getch();
	switch (c) {
		case 'q':
			return 1;
			break;
		case 's':
			return 2;
			break;
		default:
			return 0;
			break;
	}
	return 0;
}

void Usage(char* a_pProcName)
{
	std::cout << "Usage: " << std::endl << "\t " << a_pProcName << " <path to configuration file> <path to root file> <path to new log file>" << std::endl;
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

void SaveAllCanvases()
{
	std::string sImageFolderPath = Configuration::Instance().GetImageFolderPath();
	if(sImageFolderPath[sImageFolderPath.size() - 1] != '/')
	{
		sImageFolderPath += "/";
	}
	TCanvas *c = 0;
	TIter next(gROOT->GetListOfCanvases());
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	time (&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer,80,"-%F-%I:%M:%S",timeinfo);

	while ((c = (TCanvas *)next()))
	{
		std::string sFileName = sImageFolderPath;
	
		sFileName += c->GetName();
		sFileName += buffer;
		sFileName += ".png";
		c->Print(sFileName.c_str(), "png");
	}
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
	printf("blah\n");

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
//		printf("Getting entry %d\n", i);

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

	printf("press q to quit, s to save the png of all figures in %s...\n", Configuration::Instance().GetImageFolderPath().c_str());
	do
	{
		c = checkCommand();
		sigAnalyzer.ProcessEvents();
		if( c == 2 )
		{
			SaveAllCanvases();
		}
	} while(c == 0);

	sigAnalyzer.Stop();
	return 0;
}
