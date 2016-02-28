#pragma once
#include <memory>
#include <vector>
#include <map>
#include "TCanvas.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TMultiGraph.h"

typedef std::map<std::string, std::vector<int> > Range_t;
typedef std::vector<std::vector<float> > Channels_t;

class RangePlotter
{
public:
	RangePlotter();
	void PlotRanges(Channels_t& a_channels, Range_t& a_channelsToPadsAssociation, float a_samplingFreqGHz, std::string sEventTitle);
private:
	std::unique_ptr<TCanvas> m_pCanvas;
	int m_colors[16];
	std::vector <TGraph*> m_vpGraph;
	std::vector <std::unique_ptr<TMultiGraph> > m_vpMultiGraph;
	std::vector <std::unique_ptr<TLegend> > m_vpLegends;

};
