#pragma once
#include "Geometry.h"

#define NO_HIT -1
typedef std::vector<std::vector<Geometry::HorizontalRectangle3D> > CellMatrix_t;

class Panel
{
public:
	Panel(float a_fCellEfficiency, float a_fROElectrodePitchMm, 
			float a_fHVElectrodePitchMm, int a_iNumOfROElectrodes, int a_iNumOfHVElectrodes, Point3D& a_center);
private:
	float m_fCellEfficiency;
	CellMatrix_t m_cellMatrix;
	
};
