#include <chrono>
#include <unistd.h>
#include "TSystem.h"
#include "SignalAnalyzerTracker.h"
#include "TApplication.h"
#include "Configuration.h"
#include "Logger.h"
#include "keyb.h"

//using std;
using namespace std::chrono;

/**
Constructor.
//TODO: move this comment somewhere else. It does not describe the params of the function anymore...
@param a_fSamplingFreqGHz - sampling frequency in GHz.
@param a_fVoltageMin - lowest voltage in the dynamic range of the digitizer.
@param a_fVoltageMax - highest voltage in the dynamic range of the digitizer.
@param a_iDigitizerResolution - resolution of the digitizer (i.e. 0x00000fff for 12 bit).
@param a_fPulseThresholdVolts - if a sample in a channel is found below it, this channel will be considered to have a pulse.
@param a_fEdgeThresholdVolts - the time at which the signal in a channel first reaches it will be the time of the leading edge of the first pulse in that channel.
@param a_fExpectedPulseWidthNs - the size of the window after the leading edge of a pulse to look for the minimum value of the signal in (the sample with the lowest numeric value).
@param a_fMinEdgeSeparationNs - the minimum amount of time the first detected pulse in a range of channels arrives before the following one in that same range for that pulse to be considered as the original one.
@param a_fMaxEdgeJitterNs - the length of the window from the leading edge of the first pulse inside which pulses are considered to be "inseperable", that is, we cannot say just by the leading edge position which pulse comes first.
@param a_fMaxAmplitudeJitterVolts - the distance (in pulse amplitude units) from the lowest value of the largest pulse within which another pulse, if found, is not weak enough to be dismissed as a signal, and therefore all signals the lowest points of which fall within this window will be considered as potential pulses.
@param a_iDrawAftrNumOfevents - If any flags are set that tell SignalAnalyzer to plot anything, this is the number of events after which the canvases will be updated. Canvases should not be updated after every event because it's very slow.
*/
SignalAnalyzerTracker::SignalAnalyzerTracker():
m_pTriggerTimingMonitor(new TriggerTimingMonitor(milliseconds(Configuration::Instance().GetTriggerRateAveragingDurationSecs()))),
m_pTrackMonitor(new TrackMonitor()),
m_pPulseMonitor(new PulseMonitor())
{
	m_vRanges = Configuration::Instance().GetRanges();

	for (auto it : m_vRanges)
	{
		printf("Pushing P.S. for %s\n", it.first.c_str());
		m_vpPanelMonitors.push_back(std::unique_ptr<PanelMonitor>(new PanelMonitor(it.first)));
		m_vpPanelDegradationMonitors.push_back(std::unique_ptr<PanelDegradationMonitor>(new PanelDegradationMonitor(Configuration::Instance().GetPanelDegradationAveragingDurationSecs(), it.first)));
	}
	for (auto it : m_vRanges)
	{
		m_vpPanelTimingMonitors.push_back(std::unique_ptr<PanelTimingMonitor>(new PanelTimingMonitor(it.first)));
	}


}

void SignalAnalyzerTracker::DoAnalysis(nanoseconds a_timeStamp, Channels_t& a_vChannels)
{
	m_iNumberOfPanelsWithPrimaryPulse= 0;
	if (m_iFlags & AnalysisFlagsTracker::ETriggerTimingMonitor)
	{
		m_pTriggerTimingMonitor->GotTrigger(a_timeStamp);
	}
	//All these require DC offset zeroing (normalization)
	if ((m_iFlags & AnalysisFlagsTracker::EPanelHitMonitor) || (m_iFlags & AnalysisFlagsTracker::EPanelTimingMonitor) || (m_iFlags & AnalysisFlagsTracker::ETrackMonitor) || (m_iFlags & AnalysisFlagsTracker::ECountPanelsWithPrimaryPulse) || 
			(m_iFlags & AnalysisFlagsTracker::EPanelDegradationMonitor) || (m_iFlags & AnalysisFlagsTracker::EPulseMonitor))
	{
		bool bEventEmpty = false;

		if(a_vChannels.size() == 1)
		{
			if(a_vChannels[0].size() == 1)
			{
				bEventEmpty = true;
			}
		}
		
		Channels_t vNormalizedChannels;
		if(!bEventEmpty)
		{
			 vNormalizedChannels = NormalizeChannels(a_vChannels);
		}
		HitMap_t mPanelAndLine;
		SignalVector_t mSignalVector;
		int i = 0;
		for (auto it: m_vRanges)
		{
			if (!bEventEmpty)
			{
				FindOriginalPulseInChannelRange(vNormalizedChannels, it.first, it.second);
				if (m_iFlags & AnalysisFlagsTracker::EPanelHitMonitor)
				{
					m_vpPanelMonitors[i]->GotEvent(a_timeStamp, m_markers.m_vChannelsWithPulse);
				}
				if((m_iFlags & AnalysisFlagsTracker::ETrackMonitor) || (m_iFlags & AnalysisFlagsTracker::ECountPanelsWithPrimaryPulse))
				{
					if(m_markers.m_vChannelsWithPulse.size() == 1)
					{
						mPanelAndLine[it.first] = m_markers.m_vChannelsWithPulse[0];
						m_iNumberOfPanelsWithPrimaryPulse ++;
					}
					else
					{
						mPanelAndLine[it.first] = NO_SIGNAL_ON_PANEL;
					}
				}

				int iChannel = -1;
				if (m_markers.m_vChannelsWithPulse.size() == 1)
				{
					iChannel = m_markers.m_vChannelsWithPulse[0];
					if (m_iFlags & AnalysisFlagsTracker::EPulseMonitor)
					{
						mSignalVector.push_back(m_markers.m_vChannelsEdgeAndMinimum[iChannel]);
					}
					if (m_iFlags & AnalysisFlagsTracker::EPanelDegradationMonitor)
					{
						m_vpPanelDegradationMonitors[i]->GotTrigger(a_timeStamp);
					}

				}

				if(m_iFlags & AnalysisFlagsTracker::EPanelTimingMonitor)
				{
//					printf("TIMING MONITOR\n");
					DataPoint p = FindTriggerTime(vNormalizedChannels);
					if(p.Exists())
					{
/*						int cnt = 0;
						for (auto& it: m_markers.m_vChannelsEdgeAndMinimum)
						{
							printf("channel %d, edge at %f\n", cnt, std::get<EDGE_THRES_INDEX>(it).GetX());
	 						cnt++;
						}*/
//						printf("iChannel: %d, pulse x = %f\n", iChannel,  std::get<EDGE_THRES_INDEX>(m_markers.m_vChannelsEdgeAndMinimum[iChannel]).GetX());
						if((m_iFlags & AnalysisFlagsTracker::EPanelTimingMonitor) && (iChannel != -1))
						{
							m_vpPanelTimingMonitors[i]->GotEvent(iChannel, std::get<EDGE_THRES_INDEX>(m_markers.m_vChannelsEdgeAndMinimum[iChannel]).GetX(), 
								std::get<MIN_PULSE_INDEX>(m_markers.m_vChannelsEdgeAndMinimum[iChannel]).GetY(), p.GetX());
						}
					}
				}
			}
			i++;
		}
		if (m_iFlags & AnalysisFlagsTracker::ETrackMonitor)
		{
			m_pTrackMonitor->GotEvent(mPanelAndLine);
		}
		if (m_iFlags & AnalysisFlagsTracker::EPulseMonitor)
		{
			m_pPulseMonitor->GotEvent(mSignalVector);
		}
	}

	ProcessEvents();
}

void SignalAnalyzerTracker::Flush()
{
	if(m_iFlags & ETriggerTimingMonitor)
	{
		m_pTriggerTimingMonitor->Flush();
	}
}

/**
Returns the number of panels with primary pulses in an acquisition event if the flag AnalysisFlagsTracker::ECountPanelsWithPrimaryPulse is set. This is for the 'compression' utility, which goes over the raw root tree and creates a "compressed" root tree, containing only interesting events. This function and flag should be set when running in synchronous mode only (not together with the flag AnalysisFlags::EAsynchronous)!
@return Number of panels containing pulses
*/
int SignalAnalyzerTracker::GetLastNumberOfPanelsWithPrimaryPulse()
{
	return m_iNumberOfPanelsWithPrimaryPulse;
}

