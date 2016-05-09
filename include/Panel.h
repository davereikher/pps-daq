#pragma once
#include <random>
#include <vector>
#include "Geometry.h"

#define NO_HIT -1
typedef std::vector<std::vector<Geometry::HorizontalRectangle3D> > CellMatrix_t;

class Panel
{
public:
	Panel(float a_fCellEfficiency, float a_fROElectrodePitchMm, 
			float a_fHVElectrodePitchMm, int a_iNumOfROElectrodes, int a_iNumOfHVElectrodes, Geometry::Point3D a_center);

	int Captured(Geometry::Line3D a_track);
	void GenerateMatrix(float a_fROElectrodePitchMm, float a_fHVElectrodePitchMm, int a_iNumOfROElectrodes, int a_iNumOfHVElectrodes, Geometry::Point3D a_center);
	std::vector<float> GetCentersVector(int a_iNumber, float a_fPitch, float a_fTotalCenterPoint);

private:
	float m_fCellEfficiency;
	CellMatrix_t m_cellMatrix;
	std::default_random_engine m_generator;
	std::uniform_real_distribution<float> m_distribution0To1;	
};
