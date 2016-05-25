#include <math.h>
#include <cfloat>
#include "Panel.h"
#include "Cell.h"
#include "TPolyLine3D.h"
#include "TPolyMarker3D.h"
#include "Configuration.h"
#include "TPad.h"

Panel::Panel(int a_iIndex):
m_iIndex(a_iIndex),
m_fPanelZValueMm(Configuration::Instance().GetCenterZOfPanel(a_iIndex))
{
	GenerateMatrix();
}

int Panel::Captured(Geometry::Line3D& a_track)
{
	int i = 1;
	for (auto& row: m_cellMatrix)
	{
		for (auto& cell: row)
		{
			Geometry::Point3D intersectionPoint;
			if (cell.Captured(a_track))
			{
				return i;
			}
		}
		i ++;
	}
	return NO_HIT;
}

void Panel::GenerateMatrix()
{
	Configuration& config = Configuration::Instance();
	m_panelBoundaries = Geometry::HorizontalRectangle3D(config.GetNumberOfHVLinesOfPanel(m_iIndex) * config.GetXPitchOfPanel(m_iIndex) + config.GetCellXLengthOfPanel(m_iIndex),
			config.GetNumberOfROLinesOfPanel(m_iIndex) * config.GetYPitchOfPanel(m_iIndex) + config.GetCellYLengthOfPanel(m_iIndex), 
			Geometry::Point3D(config.GetCenterXOfPanel(m_iIndex), config.GetCenterXOfPanel(m_iIndex), m_fPanelZValueMm));
	std::vector<float> YCenters = GetCentersVector(config.GetNumberOfROLinesOfPanel(m_iIndex), config.GetYPitchOfPanel(m_iIndex), config.GetCenterYOfPanel(m_iIndex));
	std::vector<float> XCenters = GetCentersVector(config.GetNumberOfHVLinesOfPanel(m_iIndex), config.GetXPitchOfPanel(m_iIndex), config.GetCenterXOfPanel(m_iIndex));
//	printf("XCenters: %d, YCenter: %d", XCenters.size(), YCenters.size());
	
	std::vector<Cell> vCellRow;
	int iLineIndex = 0;
	for (auto& centerY: YCenters)
	{
		int i = 0;
		for (auto& centerX: XCenters)
		{
			Cell cell(config.GetCellEfficiencyOfPanel(m_iIndex), Geometry::HorizontalRectangle3D(config.GetCellXLengthOfPanel(m_iIndex), 
				config.GetCellYLengthOfPanel(m_iIndex), Geometry::Point3D(centerX, centerY, m_fPanelZValueMm)), iLineIndex);
			vCellRow.push_back(cell);

		}
		m_cellMatrix.push_back(vCellRow);
		vCellRow.clear();
		iLineIndex ++;
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


