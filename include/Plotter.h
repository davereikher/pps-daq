#pragma once
#include "TGraph.h"
#include "TCanvas.h"
#include "Event.h"
#include "CAENDigitizer.h"
//#include "TH1D.h"
class Plotter
{
public:
	Plotter();
	~Plotter();
	
//	void Init();
	void Plot(CAEN_DGTZ_X742_EVENT_t *evt);
//	void Plot(Event& a_event);
	std::vector<int> GenerateTime(unsigned int a_iNumOfSamples);

private:
	bool m_bInitialized;
	TCanvas* m_pCanvas;
	//TODO: use auto pointers
	std::vector<TGraph*> m_vpGraph;
//	TH1D* h;
};
