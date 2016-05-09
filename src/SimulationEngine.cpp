#include <vector>
#include "SimulationEngine.h"
#include "RandomTrackGenerator.h"
#include "Types.h"

void SimulationEngine::SingleRun()
{
	Geometry::Line3D track = m_randomTrackGenerator.GenerateTrack();
	bool bCapturedInAllScintillators = true;
	for (auto& scintillator: m_vScintillators)
	{
		if(!scintillator.Captured(track))
		{
			bCapturedInAllScintillators = false;
			break;
		}
	}
	if (!bCapturedInAllScintillators)
	{
		return;
	}
	
	//If we got here, the track intersects all scintillators and generates a signal in all of them, so the track 'triggers the DAQ'. Now we see if the track intersects and triggers signals in the panels.
	std::map<int, int> mHitMapIntegerPanelIndices;
	for (auto& panel: m_mPanels)
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

/*void SimulationEngine::Draw(TCanvas* a_pCanvas)
{
	new TGeoManager("box", "poza1");
	TGeoMaterial *mat = new TGeoMaterial("Al", 26.98,13,2.7);
	TGeoMedium *med = new TGeoMedium("MED",1,mat);
	TGeoVolume *top = gGeoManager->MakeBox("TOP",med,100,100,100);
	gGeoManager->SetTopVolume(top);
	TGeoVolume *vol = gGeoManager->MakeBox("BOX",med, 20,30,40);
	vol->SetLineWidth(2);
	top->AddNode(vol,1);
	gGeoManager->CloseGeometry();
	gGeoManager->SetNsegments(80);
	top->Draw();
	TView *view = gPad->GetView();
	view->ShowAxis();
}*/
