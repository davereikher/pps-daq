#pragma once

class SimulationEngine
{
	void Run();
private:
	std::vector<Elements::Scintillator> m_vScintillators;
	std::vector<Elements::Panel> m_vPanels;
	RandomTrackGenerator m_randomTrackGenerator;
};
