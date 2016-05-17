#include "Cell.h"

Cell::Cell(float a_fEfficiency, Geometry::HorizontalRectangle3D a_polygon, int a_iBelongingToROLine):
BasicHorizontalRectangularDetectorElement(a_fEfficiency, a_polygon),
m_iBelongingToROLine(a_iBelongingToROLine)
{
}


int Cell::GetROLine() 
{
	return m_iBelongingToROLine;
}
