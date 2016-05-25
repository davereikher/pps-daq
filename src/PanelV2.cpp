#include <math.h>
#include <cfloat>
#include "PanelV2.h"
#include "Cell.h"
#include "TPolyLine3D.h"
#include "TPolyMarker3D.h"
#include "Configuration.h"
#include "TPad.h"

PanelV2::PanelV2(int a_iIndex):
Panel(a_iIndex),
m_fGasGapsizeMm(Configuration::Instance().GetGasGapThicknessMmOfPanel(a_iIndex)),
m_fPanelZValueMm(Configuration::Instance().GetCenterZOfPanel(a_iIndex)),
m_fSigmaOfBreakdownGaussian(Configuration::Instance().GetBreakdownGenerationGaussianSigmaMmOfPanel(a_iIndex)),
m_exponentialDistribution(Configuration::Instance().GetNumberIonPairsPerMmOfPanel(a_iIndex)),
m_uniformAngleDistribution(0, 2*M_PI)
{
	std::random_device r;
	std::seed_seq seed{r(), r(), r(), r(), r(), r(), r()};
	m_generator.seed(seed);
}


bool PanelV2::WasIonized(Geometry::Line3D& a_track, Geometry::Point3D& a_point)
{
	float fPathLengthUntilIonization = m_exponentialDistribution(m_generator);

	Geometry::Point3D initialIntersectionPoint = Geometry::LineWithHorizontalPlaneIntersection(m_fPanelZValueMm, a_track);

	if( Geometry::PointExceedsBoundaries(m_panelBoundaries, initialIntersectionPoint))
	{
	//	printf("Intersection exceeds boundaries\n");
		return false;
	}

//	printf("zValue: %f, PathLengthUntilIonization: %f, pathLengthInMedium: %f ", m_fPanelZValueMm, fPathLengthUntilIonization, Geometry::GetPathLengthInHorizontalMedium(a_track, m_fGasGapsizeMm));
	if (fPathLengthUntilIonization < Geometry::GetPathLengthInHorizontalMedium(a_track, m_fGasGapsizeMm))
	{	
//		printf("Ionization occurs.. ");
		a_point = Geometry::GetPointAlongLineAtDistance(initialIntersectionPoint, a_track, fPathLengthUntilIonization);
//		printf("Boundaries: [%f, %f], [%f, %f], intersection at (%f, %f, %f), ionization at at (%f, %f, %f) ", m_panelBoundaries.GetMinX(), m_panelBoundaries.GetMaxX(), m_panelBoundaries.GetMinY(), m_panelBoundaries.GetMaxY(), initialIntersectionPoint.GetX(), initialIntersectionPoint.GetY(), initialIntersectionPoint.GetZ(), a_point.GetX(), a_point.GetY(), a_point.GetZ());
		if( !Geometry::PointExceedsBoundaries(m_panelBoundaries, a_point))
		{
//			printf("within boundaries\n");
			return true;
		}
//		printf("exceeds boundaries\n");
	}
	
//	printf("No ionization\n");
	return false;
}

int PanelV2::Captured(Geometry::Line3D& a_track)
{
	int i = 1;
	Geometry::Point3D pt;
	if (!WasIonized(a_track, pt))
	{
		return NO_HIT;
	}
	
//	printf("Panel index: %d, Hit!\n", m_iIndex);
	return GetClosestCellLine(GenerateBreakdownPoint(pt));
}

int PanelV2::GetClosestCellLine(Geometry::Point3D a_point)
{
	float fPreviousDistance = FLT_MAX;
	int iPreviousLine = m_cellMatrix[0][0].GetROLine();
	int iLine = -1;
//	printf("----");
	for (auto& line: m_cellMatrix)
	{
		iLine = line[0].GetROLine();
		//printf("Line: %d\n", iLine);
		float fYDistance = abs(a_point.GetY() - line[0].GetCenterPoint().GetY());
		//printf("distance: %f\n", fYDistance);
		if(fYDistance < fPreviousDistance)
		{
			fPreviousDistance = fYDistance;
		}
		else
		{
			break;
		}
		iPreviousLine = iLine;
	}
//	printf("****");

	return iPreviousLine;
}

Geometry::Point3D PanelV2::GenerateBreakdownPoint(Geometry::Point3D a_ionizationPoint)
{
	Geometry::Point3D pt = Geometry::GetPointAtHorizontalPolarAngleAndDistanceFrom(a_ionizationPoint, m_uniformAngleDistribution(m_generator), 
		m_fSigmaOfBreakdownGaussian * m_gaussianDistribution(m_generator));

/*	printf("Generating Breakdown Point\n");
	TPolyMarker3D * pIonization = new TPolyMarker3D(1);
	pIonization->SetPoint(0, a_ionizationPoint.GetX(), a_ionizationPoint.GetY(), a_ionizationPoint.GetZ());
	pIonization->SetMarkerStyle(8);
	pIonization->SetMarkerColor(2);
	pIonization->Draw();

	TPolyMarker3D * pBreakdown = new TPolyMarker3D(1);
	pBreakdown->SetPoint(0,pt.GetX(), pt.GetY(), pt.GetZ());
	pBreakdown->SetMarkerStyle(2);
	pBreakdown->SetMarkerColor(3);
	pBreakdown->Draw();

	gPad->Update();*/

	return pt;
}


