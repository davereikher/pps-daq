#pragma once
#include <vector>
#include <chrono>
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "Types.h"

using namespace std::chrono;

class PanelSupervisor
{
public:	
	PanelSupervisor(std::string a_sPanelName);
	void GotEvent(nanoseconds a_eventTime, std::vector<int> a_vChannelsWithPulse);
	void GotTrigger();
private:
	void InitGraphics();
	void AddToChannelsHistogram(std::vector<int>& a_vChannels);
	void AddToSimultaneousChannelsPlot(nanoseconds a_eventTime, int a_iNumberOfSimultaneousChannels);
private:
	std::unique_ptr<TCanvas> m_pCanvas;
	TGraph* m_pSimultaneousChannelGraph;
	TH1* m_pChannelsHist;
	std::string m_sPanelName;
	int m_iTriggerCount;
	int m_iEventCount;
};
