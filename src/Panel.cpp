#include "Panel.h"

int Panel::Captured(Line3D& a_track)
{
	int i = 1;
	for (auto& row: m_cellMatrix)
	{
		for (auto& cell: row)
		{
			Point intersectionPoint;
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

void Panel::GenerateMatrix()
{
	std::vector<float> ROCenters = GetCentersVector(a_iNumOfROElectrodes, a_fROElectrodePitchMm, a_center.GetY());
	std::vector<float> HVCenters = GetCentersVector(a_iNumOfHVElectrodes, a_fHVElectrodePitchMm, a_center.GetX());
	
	std::vector<Geometry::HorizontalRectangle3D> vCellRow;
	for (auto& center: HVCenters)
	{
		vCellRow.push_back(TODO);
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
