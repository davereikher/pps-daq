#pragma once
#include <memory>
#include "TGraph.h"
#include "TCanvas.h"
#include "Event.h"
#include "CAENDigitizer.h"
//#include "TH1D.h"
class Plotter
{
public:
	Plotter(int argc, char** argv);
	~Plotter();
	
	void Plot(CAEN_DGTZ_X742_EVENT_t *evt);
	std::vector<float> GenerateTime(unsigned int a_iNumOfSamples);

private:
	bool m_bInitialized;
	std::unique_ptr<TCanvas> m_pCanvas;
	std::vector<TGraph*> m_vpGraph;
};
