#pragma once
#include "Geometry.h"
#include "BasicHorizontalRectangularDetectorElement.h"

class Cell: public BasicHorizontalRectangularDetectorElement
{
public:
	Cell(float a_fEfficiency, Geometry::HorizontalRectangle3D a_polygon, int a_iBelongingToROLine);
	bool Captured(Geometry::Line3D& a_track);

	int GetROLine() {return m_iBelongingToROLine;}
private:
	int m_iBelongingToROLine;
};
