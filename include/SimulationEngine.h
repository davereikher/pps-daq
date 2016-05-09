#pragma once
#include <vector>
#include <map>
#include "Types.h"
#include "Scintillator.h"
#include "Panel.h"
#include "RandomTrackGenerator.h"

class SimulationEngine
{
	void SingleRun();
	HitMap_t GetResults();
private:
	std::vector<Scintillator> m_vScintillators;
	std::map<std::string, Panel> m_mPanels;
	RandomTrackGenerator m_randomTrackGenerator;
	HitMap_t m_mResult;
};
