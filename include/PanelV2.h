#pragma once
#include <random>
#include <vector>
#include "Geometry.h"
#include "Cell.h"
#include "Panel.h"

#define NO_HIT -1
typedef std::vector<std::vector<Cell> > CellMatrix_t;

class PanelV2: public Panel
{
public:
	PanelV2(int a_iIndex);
	PanelV2(){}

	int Captured(Geometry::Line3D& a_track);
	bool WasIonized(Geometry::Line3D& a_track, Geometry::Point3D& a_point);
	int GetClosestCellLine(Geometry::Point3D a_point);
	Geometry::Point3D GenerateBreakdownPoint(Geometry::Point3D a_ionizationPoint);

private:
	float m_fGasGapsizeMm;
	float m_fPanelZValueMm;
	float m_fSigmaOfBreakdownGaussian;
	std::default_random_engine m_generator;
	std::exponential_distribution<float> m_exponentialDistribution;
	std::uniform_real_distribution<float> m_uniformAngleDistribution;
	std::normal_distribution<float> m_gaussianDistribution;
};
