#include <vector>
#include "SimulationEngine.h"
#include "RandomTrackGenerator.h"
#include "Types.h"
#include "Configuration.h"
#include "TGeoManager.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoVolume.h"
#include "TView.h"
#include "TPolyLine3D.h"


SimulationEngine::SimulationEngine(RandomTrackGenerator& a_rg):
m_randomTrackGenerator(a_rg)
{
	InitGraphics();
	InitObjects();
}

void SimulationEngine::InitGraphics()
{	
	m_pCanvas = std::unique_ptr<TCanvas>(new TCanvas("TrackerSim", "Tracker Simulation", 800, 600));
	m_pCanvas->Draw();
}

void SimulationEngine::InitObjects()
{
	Configuration& config = Configuration::Instance();
	Scintillator topScint(config.GetTopScintillatorEfficiency(),
			Geometry::HorizontalRectangle3D(config.GetTopScintillatorLengthXMm(),	config.GetTopScintillatorLengthYMm(),
			config.GetTopScintillatorCenterXMm(), config.GetTopScintillatorCenterYMm(), config.GetTopScintillatorCenterZMm()));
	Scintillator bottomScint(config.GetBottomScintillatorEfficiency(),
			Geometry::HorizontalRectangle3D(config.GetBottomScintillatorLengthXMm(),	config.GetBottomScintillatorLengthYMm(),
			config.GetBottomScintillatorCenterXMm(), config.GetBottomScintillatorCenterYMm(), config.GetBottomScintillatorCenterZMm()));
	
	m_vScintillators.push_back(topScint);
	m_vScintillators.push_back(bottomScint);
}

void SimulationEngine::SingleRun()
{
	Geometry::Line3D track = m_randomTrackGenerator.GenerateTrack();
	DrawTrack(track);
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

float SimulationEngine::GetMaxZ()
{
	return 500;
}

float  SimulationEngine::GetMinZ()
{
	return -500;
}

void SimulationEngine::DrawTrack(Geometry::Line3D& a_track)
{
	Geometry::Point3D top = Geometry::LineWithHorizontalPlaneIntersection(GetMaxZ(), a_track);
	Geometry::Point3D bottom = Geometry::LineWithHorizontalPlaneIntersection(GetMinZ(), a_track);
	
	m_pCanvas->cd();

	TPolyLine3D* pPolyLine = new TPolyLine3D(2);
	printf("Bottom point: %f, %f, %f\nTop point: %f, %f, %f\n--\n", bottom.GetX(), bottom.GetY(), bottom.GetZ(), top.GetX(), top.GetY(), top.GetZ());

	pPolyLine->SetPoint(0, top.GetX(), top.GetY(), top.GetZ());
//	pPolyLine->SetPoint(0, -11.362981, -2.280738, -10.000000);
	pPolyLine->SetPoint(1, bottom.GetX(), bottom.GetY(), bottom.GetZ());
//	pPolyLine->SetPoint(1, -10.744752, -19.826996, 10.000000);
	pPolyLine->SetLineWidth(1);
	pPolyLine->SetLineColor(2);
	pPolyLine->Draw();
	m_pCanvas->Update();
}

HitMap_t SimulationEngine::GetResults()
{
	return m_mResult;
}

void SimulationEngine::Draw()
{
	m_pCanvas->cd();
/*	new TGeoManager("box", "poza1");
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
	view->ShowAxis();*/
	for (auto& it: m_vScintillators)
	{
		TPolyLine3D* scint = it.GeneratePolyLine3D();
		scint->SetLineWidth(3);
		scint->SetLineColor(4);
		scint->Draw();

	}
/*	TPolyLine3D*  = new TPolyLine3D(5);
	pPolyLine->SetPoint(0, -10, -10, 0);
	pPolyLine->SetPoint(1, 10, -10, 0);
	pPolyLine->SetPoint(2, 10, 10, 0);
	pPolyLine->SetPoint(3, -10, 10, 0);
	pPolyLine->SetPoint(4, -10, -10, 0);
	pPolyLine->SetFillColor(38);
	pPolyLine->SetLineWidth(3);
	pPolyLine->SetLineColor(3);
	pPolyLine->Draw();*/
	m_pCanvas->Update();
}

void SimulationEngine::Wait()
{
	m_pCanvas->WaitPrimitive();
}

