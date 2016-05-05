#include <cmath>
#include "Geometry.h"

bool LineWithHorizontalRectangleIntersection(HorizontalRectangle3D& a_rectangle, Line3D& a_line, Point3D& a_intersection)
{
	Point pt(LineWithHorizontalPlaneIntersection(a_rectangle.GetZOffset(), a_line);
	if (
		(pt.GetX() < a_rectangle.GetMinX()) ||
		(pt.GetX() > a_rectangle.GetMaxX()) ||
		(pt.GetY() > a_rectangle.GetMaxY()) ||
		(pt.GetY() < a_rectangle.GetMinY())
	{
		return false;
	}

	a_intersection = pt;
	return true;
}

Point LineWithHorizontalPlaneIntersection(float a_fPlaneZValue, Line3D& a_line)
{
	float fT = (a_fPlaneZValue - a_line.GetPoint().GetZ())/cos(a_line.GetTheta());
	float fX = a_line.GetPoint().GetX() + sin(a_line.GetTheta()) * cos(a_line.GetPhi()) * fT;
	float fY = a_line.GetPoint().GetX() + sin(a_line.GetTheta()) * sin(a_line.GetPhi()) * fT;
	return Point(fX, fY, a_fPlaneZValue);
}
