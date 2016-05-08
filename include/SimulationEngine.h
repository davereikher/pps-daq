#pragma once

class SimulationEngine
{
	void SingleRun();
private:
	std::vector<Scintillator> m_vScintillators;
	std::map<std::string, Panel> m_mPanels;
	RandomTrackGenerator m_randomTrackGenerator;
	HitMap_t m_mResult;
};
