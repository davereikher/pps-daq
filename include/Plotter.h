#pragma once
#include "TGraph.h"
#include "TCanvas.h"
#include "Event.h"

class Plotter
{
public:
	Plotter();
	~Plotter();
	
	void Init();
	void Plot(Event& a_event);

private:
	TCanvas* m_pCanvas;
	TGraph * m_pGraph;
};
