#pragma once
#include "Geometry.h"
#include "BasicHorizontalRectangularDetectorElement.h"

class Cell: public BasicHorizontalRectangularDetectorElement
{
public:
	Cell(float a_fEfficiency, Geometry::HorizontalRectangle3D a_polygon);
	bool Captured(Geometry::Line3D& a_track);
};
