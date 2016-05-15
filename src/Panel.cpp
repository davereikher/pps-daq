#include "Panel.h"
#include "Cell.h"
#include "TPolyLine3D.h"
#include "Configuration.h"

Panel::Panel(int a_iIndex):
m_iIndex(a_iIndex),
m_fCellEfficiency(Configuration::Instance().GetCellEfficiencyOfPanel(a_iIndex)),
m_fGasGapsizeMm(Configuration::Instance().GetGasGapThicknessMm()),
m_fPanelZValueMm(Configuration::Instance().GetCenterZOfPanel(a_iIndex)),
m_exponentialDistribution(Configuration::Instance().GetNumberIonPairsPerMmOfPanel())
{
	std::random_device r;
	std::seed_seq seed{r(), r(), r(), r(), r(), r(), r()};
	m_generator.seed(seed);

	GenerateMatrix();
}


bool Panel::WasIonized(Geometry::Line3D a_track, Geometry::Point& a_point)
{
	float fPathLengthUntilIonization = exponential_distribution(m_generator);
	if (fPathLengthUntilIonization < Geometry::GetPathLengthInHorizontalMedium(a_line, m_fGasGapsizeMm))
	{	
		a_point = Geometry::LineWithHorizontalPlaneIntersection(m_fPanelZValueMm + m_fGasGapsizeMm, a_track);
		if( !Geometry::PointExceedsBoundaries(m_panelBoundaries, a_point))
		{
			return true;
		}
	}
	return false;
}

/*int Panel::Captured(Geometry::Line3D a_track)
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
}*/

int Panel::Captured(Geometry::Line3D a_track)
{
	int i = 1;
	Geometry::Point3D pt;
	if (!WasIonized(a_track, pt))
	{
		return NO_HIT;
	}
	
	

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

int Panel::GetClosestCellLine(Geometry::Point3D a_point)
{
	float fPreviousDistance = FLT_MAX;
	int iLine = 0;
	for (auto& line: m_cellMatrix)
	{
		iLine = line[0].GetLine();
		float fYDistance = abs(a_point.GetY() - iLine);
		if(fYDistance < fPreviousDistance)
		{
			fPreviousDistance = fYDistance;
		}
		else
		{
			break;
		}
	}

	return iLine;
}

void Panel::GenerateMatrix()
{
	Configuration& config = Configuration::Instance();
	m_panelBoundaries = Geometry::HorizontalRectangle3D(config.GetNumberOfHVLinesOfPanel(m_iIndex) * config.GetXPitchOfPanel(m_iIndex) + config.GetCellXLengthOfPanel(m_iIndex),
			config.GetNumberOfROLinesOfPanel(m_iIndex) * config.GetYPitchOfPanel(m_iIndex) + config.GetCellYLengthOfPanel(m_iIndex), 
			Geometry::Point3D(config.GetCenterXOfPanel(m_iIndex), config.GetCenterXOfPanel(m_iIndex), m_fPanelZValueMm));
	std::vector<float> YCenters = GetCentersVector(config.GetNumberOfROLinesOfPanel(m_iIndex), config.GetYPitchOfPanel(m_iIndex), config.GetCenterYOfPanel(m_iIndex));
	std::vector<float> XCenters = GetCentersVector(config.GetNumberOfHVLinesOfPanel(m_iIndex), config.GetXPitchOfPanel(m_iIndex), config.GetCenterXOfPanel(m_iIndex));
	printf("XCenters: %d, YCenter: %d", XCenters.size(), YCenters.size());
	
	std::vector<Cell> vCellRow;
	int iLineIndex = 0;
	for (auto& centerY: YCenters)
	{
		for (auto& centerX: XCenters)
		{

			Cell cell(config.GetCellEfficiencyOfPanel(m_iIndex), Geometry::HorizontalRectangle3D(config.GetCellXLengthOfPanel(m_iIndex), 
				config.GetCellYLengthOfPanel(m_iIndex), Geometry::Point3D(centerX, centerY, m_fPanelZValueMm)), iLineIndex);
		
			vCellRow.push_back(cell);

		}
		m_cellMatrix.push_back(vCellRow);
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



