#include <stdio.h>
#include "Plotter.h"

Plotter::Plotter(int argc, char **argv):
m_bInitialized(false)
{
	m_vpGraph.resize(4);
//	Init();
}
/*
void Plotter::Init()
{
	m_pCanvas = new TCanvas("c1", "Title", 800, 600);
	h = new TH1D ("h", "h", 10, 0, 10);
	h->Fill(1);
	h->Draw();	
	m_pCanvas->Draw();

//	m_pGraph->Draw("ACP");
}
*/
void Plotter::Plot(CAEN_DGTZ_X742_EVENT_t *evt)
{
	//#define TP printf("TP: %d\n", __LINE__)
	#define TP
	#define DATA_CHANNEL 0
	TP;
	if(!m_bInitialized)
	{
		printf("NOT INITIALIZED\n");
		m_pCanvas.reset(new TCanvas("m_pCanvas", "Title", 800, 600));
		TP;
		m_pCanvas->Divide(2,2);
		TP;
		for (int i = 0; i < 4; i++)
		{
			if (!evt->GrPresent[i])
			{
				continue;
			}
			TP;
			printf("iter: %d", i);
			m_pCanvas->cd(i+1);
			TP;
			TGraph* pGr = new TGraph(1024, (Float_t*)GenerateTime(1024).data(), (Float_t*)evt->DataGroup[i].DataChannel[DATA_CHANNEL]);
			TP;
			m_vpGraph[i] = pGr;
			TP;
			pGr->Draw("ACP");
		}
		m_pCanvas->Draw();
		m_bInitialized = true;
	}
	else
	{
		m_pCanvas->Clear();
		m_pCanvas->Divide(2,2);
		for (int i = 0; i < 4; i ++)
		{
			if (!evt->GrPresent[i])
			{
				continue;
			}
			m_pCanvas->cd(i+1);
			m_vpGraph[i]->DrawGraph(1024, (Float_t*)GenerateTime(1024).data(), (Float_t*)evt->DataGroup[i].DataChannel[DATA_CHANNEL], "ACP");
		}
		m_pCanvas->Update();
	}

//	printf("ev len: %d, vector data len: %d\n", ev.m_vGroupEvents[0].m_iSizeOfEachChannelSamples, ev.m_vGroupEvents[0].m_vChannelSamples[0].size());
//	m_pGraph->Draw("ACP");
}

Plotter::~Plotter()
{
	for (int i = 0; i < (int)m_vpGraph.size(); i++)
	{
		delete m_vpGraph[i];
	}
}

//TODO: add frequency parameter
std::vector<float> Plotter::GenerateTime(unsigned int a_iNumOfSamples)
{
//	long iTrueFrequency = ;
//	switch (a_iFrequency
//	double time_step = 1.0/a_iFrequency;
	std::vector<float> vResult;
	vResult.resize(a_iNumOfSamples);
	for (int i = 0; i < (int)a_iNumOfSamples; i++)
	{
		vResult[i] = i * 0.4;
	}
	return vResult;
}

