#include "Panel.h"

Panel::Panel(float a_fCellEfficiency, float a_fROElectrodePitchMm, 
			float a_fHVElectrodePitchMm, int a_iNumOfROElectrodes, int a_iNumOfHVElectrodes, Geometry::Point3D a_center):
m_fCellEfficiency(a_fCellEfficiency)
{
	GenerateMatrix(a_fROElectrodePitchMm, a_fHVElectrodePitchMm, a_iNumOfROElectrodes, a_iNumOfHVElectrodes, a_center);
}


int Panel::Captured(Geometry::Line3D a_track)
{
	int i = 1;
	for (auto& row: m_cellMatrix)
	{
		for (auto& cell: row)
		{
			Geometry::Point3D intersectionPoint;
			if (Geometry::LineWithHorizontalRectangleIntersection(cell, a_track, intersectionPoint))
			{
				if (m_distribution0To1(m_generator) <= m_fCellEfficiency)
				{
					return i;
				}
				return NO_HIT;
			}
		}
		i ++;
	}

	return NO_HIT;
}

void Panel::GenerateMatrix(float a_fROElectrodePitchMm, float a_fHVElectrodePitchMm, int a_iNumOfROElectrodes, int a_iNumOfHVElectrodes, Geometry::Point3D a_center)
{
	std::vector<float> YCenters = GetCentersVector(a_iNumOfROElectrodes, a_fROElectrodePitchMm, a_center.GetY());
	std::vector<float> XCenters = GetCentersVector(a_iNumOfHVElectrodes, a_fHVElectrodePitchMm, a_center.GetX());
	
	std::vector<Geometry::HorizontalRectangle3D> vCellRow;
	for (auto& centerY: YCenters)
	{
		for (auto& centerX: XCenters)
		{
			vCellRow.push_back(Geometry::HorizontalRectangle3D(a_fHVElectrodePitchMm, a_fROElectrodePitchMm, centerX, centerY, a_center.GetZ()));
		}
		m_cellMatrix.push_back(vCellRow);
	}
}

std::vector<float> Panel::GetCentersVector(int a_iNumber, float a_fPitch, float a_fTotalCenterPoint)
{
	float totalWidth = a_iNumber * a_fPitch;
	float fLowestBorder = a_fTotalCenterPoint - totalWidth/2;
	std::vector<float> vCentersVector;
	for (int i = 0; i < a_iNumber; i++)
	{
		vCentersVector.push_back(fLowestBorder + i * a_fPitch + a_fPitch/2);
	}
}

