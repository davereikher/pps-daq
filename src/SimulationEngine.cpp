#include "SimulationEngine.h"
#include "RandomTrackGenerator.h"
#include "Types.h"

void SimulationEngine::Run(int a_iNumOfIterations)
{
	std::vector<Line3D> vTracks = m_randomTrackGenerator::GenerateTracks(a_iNumOfIterations);
		
}

HitMap_t SimulationEngine::GetResults()
{
	
}
