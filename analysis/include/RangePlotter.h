#pragma once
#include <memory>
#include <vector>
#include <map>
#include "TCanvas.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "SignalAnalyzer.h"

typedef std::map<std::string, std::vector<int> > Range_t;
typedef std::vector<std::vector<float> > Channels_t;

class RangePlotter
{
public:
	RangePlotter(float a_fSamplingFreqGHz, float a_fMinVoltage, float a_fMaxVoltage, int a_iDigitizerResolution);
	void PlotRanges(Channels_t& a_channels, Range_t& a_channelsToPadsAssociation, std::string sEventTitle);
	void AddAnalysisMarkers(int a_iPanelIndex, SignalAnalyzer::AnalysisMarkers& a_analysisMarkers);
	void WaitForDoubleClick();
private:
	std::vector<float> TransformToVoltage(std::vector<float> a_vSamples);
private:
	std::unique_ptr<TCanvas> m_pCanvas;
	int m_colors[16];
	float m_fSamplingFreqGHz;
	float m_fMinVoltage;
	float m_fVoltageDivision;
	std::vector <TGraph*> m_vpGraph;
	std::vector <std::unique_ptr<TMultiGraph> > m_vpMultiGraph;
	std::vector <std::unique_ptr<TLegend> > m_vpLegends;
};
