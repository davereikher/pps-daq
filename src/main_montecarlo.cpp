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
#include "TStyle.h"
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

void SetPlotStyle()
{
	gStyle->SetLabelSize(0.06, "xyz");
	gStyle->SetTitleSize(0.06, "xyz");
	gStyle->SetTitleOffset(1.1, "xyz");
	gStyle->SetTitleFontSize(0.08);
	gStyle->SetOptStat("e");
	gStyle->SetStatW(0.3);
	gStyle->SetStatH(0.3);
	gStyle->SetPadLeftMargin(0.15);
	gStyle->SetPadRightMargin();
	gStyle->SetPadTopMargin(0.15);
	gStyle->SetPadBottomMargin(0.15);
}

int main(int argc, char* argv[])
{
	TApplication* pApplication = new TApplication("app",0, 0);

	if (argc < 2)
	{
		Usage(argv[0]);
		exit(-1);
	}

	Configuration& config = Configuration::Instance();
	SetPlotStyle();

	config.LoadConfiguration(argv[1]);

	RandomTrackGenerator rg(Geometry::HorizontalRectangle3D(config.GetMonteCarloPointDomainRectangleLengthXMm(),
			config.GetMonteCarloPointDomainRectangleLengthYMm(), 
			Geometry::Point3D(config.GetMonteCarloPointDomainRectangleCenterXMm(), config.GetMonteCarloPointDomainRectangleCenterYMm(), config.GetMonteCarloPointDomainRectangleCenterZMm())));

/*	TCanvas pCanvas("test_distributions", "Test distributions", 800, 600);
	pCanvas.Divide(1,2);
	TH1F pPhiHist("Phi" , "Phi", 100, 0, 0);
	pPhiHist.SetFillColor(49);
	TH1F pThetaHist("Phi" , "Phi", 100, 0, 0);
	pThetaHist.SetFillColor(49);
*/	
	SimulationEngine simEngine(rg);
	simEngine.Draw();	
	SignalAnalyzer sigAnalyzer;
	int iHitCounter[] = {0,0,0,0,0};
	int iNumberOfRuns = Configuration::Instance().GetNumberOfMonteCarloRuns();
	for (int i = 0; i < iNumberOfRuns; i++)
	{
		ShowProgress(i, iNumberOfRuns);
		simEngine.SingleRun(SimulationEngine::ERandomPulses);
		HitMap_t& results = simEngine.GetResults();
		bool bWritten = false;
	/*	for (auto& it: results)
		{
			bWritten = true;
			printf("(%s, %d) ", it.first.c_str(), it.second);
		}
		if(bWritten)
		{
			printf("\n");
		}*/
		iHitCounter[results.size()]++;
		if(results.size() >= 2)
		{
			sigAnalyzer.AnalyzeTrack(results);
//			printf("----");
//			simEngine.Wait();
		}
		char c = checkCommand();
		if (c == 1) break;

/*		float sum = iHitCounter[0] + iHitCounter[1] + iHitCounter[2] + iHitCounter[3];
		std::cout << (iHitCounter[0]/sum)*100 << "\t\t"<< (iHitCounter[1]/sum)*100<<"\t\t" << (iHitCounter[2]/sum)*100<<"\t\t" << (iHitCounter[3]/sum)*100 << "                         \r";
		std::cout.flush();*/
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

	config.LoadConfiguration(argv[1]);

	std::string sRootFileName(argv[2]);
	
	Logger::Instance().Init(argv[3]);
	Logger::Instance().AddNecessaryMessage(config.GetDump());

	SignalAnalyzer sigAnalyzer;

	sigAnalyzer.SetFlags(SignalAnalyzer::ETrackMonitor);

	sigAnalyzer.Start();
	
	Range_t mRanges = config.GetRanges();

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
