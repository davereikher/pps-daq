#include "Panel.h"
#include "Cell.h"
#include "TPolyLine3D.h"
#include "Configuration.h"

Panel::Panel(int a_iIndex):
m_iIndex(a_iIndex),
m_fCellEfficiency(Configuration::Instance().GetCellEfficiencyOfPanel(a_iIndex))
{
	GenerateMatrix();
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

void Panel::GenerateMatrix()
{
	Configuration& config = Configuration::Instance();
	std::vector<float> YCenters = GetCentersVector(config.GetNumberOfROLinesOfPanel(m_iIndex), config.GetYPitchOfPanel(m_iIndex), config.GetCenterYOfPanel(m_iIndex));
	std::vector<float> XCenters = GetCentersVector(config.GetNumberOfHVLinesOfPanel(m_iIndex), config.GetXPitchOfPanel(m_iIndex), config.GetCenterXOfPanel(m_iIndex));
	printf("XCenters: %d, YCenter: %d", XCenters.size(), YCenters.size());
	
	std::vector<Cell> vCellRow;
	for (auto& centerY: YCenters)
	{
		
		for (auto& centerX: XCenters)
		{

			Cell cell(config.GetCellEfficiencyOfPanel(m_iIndex), Geometry::HorizontalRectangle3D(config.GetCellXLengthOfPanel(m_iIndex), 
				config.GetCellYLengthOfPanel(m_iIndex), Geometry::Point3D(centerX, centerY, config.GetCenterZOfPanel(m_iIndex))));
		
			vCellRow.push_back(cell);

		}
		m_cellMatrix.push_back(vCellRow);
	}
}

std::vector<float> Panel::GetCentersVector(int a_iNumber, float a_fPitch, float a_fGlobalCenterPoint)
{
	float totalWidth = a_iNumber * a_fPitch;
	float fLowestBorder = a_fGlobalCenterPoint - totalWidth/2;
	std::vector<float> vCentersVector;
	for (int i = 0; i < a_iNumber; i++)
	{
		vCentersVector.push_back(fLowestBorder + i * a_fPitch + a_fPitch/2);
	}
	return vCentersVector;
}

void Panel::Draw()
{
	for (auto& cellRow: m_cellMatrix)
	{
		for (auto& cell: cellRow)
		{
			TPolyLine3D* polyline = cell.GeneratePolyLine3D();
			polyline->SetLineWidth(2);
			polyline->SetLineColor(4);
			polyline->Draw();
		}
	}
}

