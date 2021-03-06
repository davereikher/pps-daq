#pragma once
#include <memory>
#include <vector>
#include <map>
#include "TCanvas.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "SignalAnalyzer.h"
#include "Types.h"

class RangePlotter
{
public:
	RangePlotter(float a_fSamplingFreqGHz, float a_fMinVoltage, float a_fMaxVoltage, int a_iDigitizerResolution, std::string a_sInstanceName);
	void PlotRanges(Channels_t& a_channels, Range_t& a_channelsToPadsAssociation, std::string sEventTitle);
	void AddAnalysisMarkers(int a_iPanelIndex, SignalAnalyzer::AnalysisMarkers& a_analysisMarkers);
	void Wait();
private:
	inline float TransformToVoltage(float a_fSample);
	void MakePads(int a_iChannelsToPadsAssociationSize);
	void ChangePad(int a_iIndex);

private:
	std::unique_ptr<TCanvas> m_pCanvas;
	std::vector<std::unique_ptr<TCanvas> > m_vCanvasVector;
	int m_colors[16];
	float m_fSamplingFreqGHz;
	float m_fMinVoltage;
	float m_fMaxVoltage;
	float m_fVoltageDivision;
	std::map <int, TGraph*> m_vpGraph;
	TGraph* m_vpGraphPrecisionTrigger;
	std::vector <std::unique_ptr<TMultiGraph> > m_vpMultiGraph;
	std::vector <std::unique_ptr<TLegend> > m_vpLegends;
	std::string m_sInstanceName;
};
