#include <cmath>
#include "stdio.h"
#include "Geometry.h"

bool Geometry::LineWithHorizontalRectangleIntersection(Geometry::HorizontalRectangle3D a_rectangle, Line3D a_line, Geometry::Point3D& a_intersection)
{
	Point3D pt(LineWithHorizontalPlaneIntersection(a_rectangle.GetZOffset(), a_line));
	if (
		(pt.GetX() < a_rectangle.GetMinX()) ||
		(pt.GetX() > a_rectangle.GetMaxX()) ||
		(pt.GetY() > a_rectangle.GetMaxY()) ||
		(pt.GetY() < a_rectangle.GetMinY()))
	{
		return false;
	}

	a_intersection = pt;
	return true;
}

Geometry::Point3D Geometry::LineWithHorizontalPlaneIntersection(float a_fPlaneZValue, Geometry::Line3D& a_line)
{
//	printf("a_line.GetPoint: %f\n", a_line.GetPoint().GetX());
	float fT = (a_fPlaneZValue - a_line.GetPoint().GetZ())/cos(a_line.GetTheta());
	float fX = a_line.GetPoint().GetX() + sin(a_line.GetTheta()) * cos(a_line.GetPhi()) * fT;
	float fY = a_line.GetPoint().GetY() + sin(a_line.GetTheta()) * sin(a_line.GetPhi()) * fT;
	return Point3D(fX, fY, a_fPlaneZValue);
}

