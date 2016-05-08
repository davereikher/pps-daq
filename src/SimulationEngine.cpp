#include "SimulationEngine.h"
#include "RandomTrackGenerator.h"
#include "Types.h"

void SimulationEngine::SingleRun()
{
	Line3D track = m_randomTrackGenerator.GetRandomTrack();
	bool bCapturesInAllScintillators = true;
	for (auto& scintillator: m_vScintillators)
	{
		if(!scintillator.Captured(track))
		{
			bCapturedInAllScintillators = false;
			break;
		}
	}
	if (!bCapturesInAllScintillators)
	{
		continue;
	}
	
	//If we got here, the track intersects all scintillators and generates a signal in all of them, so the track 'triggers the DAQ'. Now we see if the track intersects and triggers signals in the panels.
	std::map<int, int> mHitMapIntegerPanelIndices;
	int iPanelCounter = 0;
	for (auto& panel: m_vPanels)
	{
		int iLine = panel.second.Captured(track);
		if (iLine != -1)
		{
			m_mResult[panel.first] = iLine;
		}
	}
}

HitMap_t SimulationEngine::GetResults()
{
	return m_mResult;
}
