#pragma once
#include <memory>
#include <vector>
#include <map>
#include "TCanvas.h"
#include "Types.h"
#include "Scintillator.h"
#include "Panel.h"
#include "RandomTrackGenerator.h"

class SimulationEngine
{
public:
	SimulationEngine(RandomTrackGenerator& a_rd);
	void SingleRun();
	HitMap_t& GetResults();
	void Draw();
	TPolyLine3D* MakePolyLine(Geometry::Line3D& a_track);
	void DrawPolyLine(TPolyLine3D* a_pPolyLine);
	float GetMinZ();
	float GetMaxZ();
	void Wait();

private:
	void InitGraphics();
	void InitObjects();
private:
	std::vector<Scintillator> m_vScintillators;
	std::map<std::string, Panel> m_mPanels;
	RandomTrackGenerator& m_randomTrackGenerator;
	HitMap_t m_mResult;
	std::unique_ptr<TCanvas> m_pCanvas;
};
