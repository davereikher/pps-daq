#include "Plotter.h"

Plotter::Plotter()
{
	Init();
}

void Plotter::Init()
{
	m_pCanvas = new TCanvas("c1", "Title", 800, 600);
	m_pGraph = new TGraph();
}

void Plotter::Plot(Event& ev)
{
	m_pGraph->DrawGraph((Int_t)ev.m_vGroupEvents[0].m_iSizeOfEachChannelSamples, (Int_t*)ev.m_vGroupEvents[0].m_vChannelSamples[0].data(), (Int_t*)ev.m_vGroupEvents[0].m_vChannelSamples[0].data());
}

Plotter::~Plotter()
{
	delete m_pGraph;
	delete m_pCanvas;
}

