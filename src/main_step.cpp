#include <iostream>
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TTimer.h"
#include "TSystem.h"
#include "TApplication.h"
#include "RangePlotter.h"
#include "Configuration.h"
#include "SignalAnalyzerMicrocavity.h"

void Usage(char* a_pProcName)
{
	std::cout << "Usage: " << std::endl << "\t " << a_pProcName << " <path to configuration file> <path to root file> <start index>" << std::endl;
}

void SaveAllCanvases(int a_iEventNumber)
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
		sFileName += std::to_string(a_iEventNumber);
		sFileName += ".png";
		c->Print(sFileName.c_str(), "png");
	}
}


/**
step through the events in the root file. For each event, plot all active channels, analyse and write result to stdout
*/
int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		Usage(argv[0]);
		exit(-1);
	}

	Configuration::Instance().LoadConfiguration(argv[1]);

	std::string sRootFileName(argv[2]);
	SignalAnalyzerMicrocavity sigAnalyzer;

	//The constructor of TApplication causes a segmentation violation, so we instantiate it on the heap and not delete it at the end. This is bad, but not fatal.
	TApplication* pApplication = new TApplication("app",&argc,argv);
	RangePlotter plt(Configuration::Instance().GetSamplingFreqGHz(), Configuration::Instance().GetVoltMin(), Configuration::Instance().GetVoltMax(),
		Configuration::Instance().GetDigitizerResolution(), "Normalized");
	
	Range_t ranges = Configuration::Instance().GetRanges();

	Channels_t * channels = NULL;

	TFile f(sRootFileName.c_str());
	TTree* tree = (TTree*)f.Get("DigitizerEvents");
	tree->SetBranchAddress("Event", &channels);

	int iNumOfEntries = tree->GetEntries();
	int iStartingIndex = std::stoi(argv[3]);
	for (int i = iStartingIndex; i < iNumOfEntries; i ++)
	{
		tree->GetEntry(i);
		if( (*channels)[0].size() > 1)
		{
			Channels_t vNormalizedChannels = sigAnalyzer.NormalizeChannels(*channels);
		
			plt.PlotRanges(vNormalizedChannels, ranges, std::string("Event  ") + std::to_string(i)); 
			sigAnalyzer.FindOriginalPulseInChannelRange(vNormalizedChannels, "E", ranges["E"]);
			plt.AddAnalysisMarkers(0, sigAnalyzer.GetAnalysisMarkers());

			//---To save all collected figures uncomment this and then make -f make_step in ~/pps/pps_daq. Images are saved in the field "image-folder-path" in configuration
			SaveAllCanvases(i);
		
			//---To browse through collected figures one by one uncomment this and then make -f make_step in ~/pps/pps_daq
//			plt.Wait();
		}

	}

	return 0;
}


