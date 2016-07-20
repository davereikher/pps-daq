#include "BasicHorizontalRectangularDetectorElement.h"

BasicHorizontalRectangularDetectorElement::BasicHorizontalRectangularDetectorElement(float a_fEfficiency, HorizontalRectangle3D a_polygon):
	HorizontalRectangle3D(a_polygon),
	m_fEfficiency(a_fEfficiency)
{
}

bool BasicHorizontalRectangularDetectorElement::Captured(Geometry::Line3D& a_track)
{
	Geometry::Point3D intersection;
	if (Geometry::LineWithHorizontalRectangleIntersection(*this, a_track, intersection))
	{
		if (m_distribution0To1(m_generator) <= m_fEfficiency)
		{
//			printf("Captured!\n");
			return true;
		}
	}
}

TPolyLine3D* BasicHorizontalRectangularDetectorElement::GeneratePolyLine3D()
{
	TPolyLine3D* pPolyLine = new TPolyLine3D(5);
//	printf("---------MinX: %f, Maxx:%f, Miny: %f, MaxY:%f, Z: %f\n", GetMinX(), GetMaxX(), GetMinY(), GetMaxY(), GetZOffset());
	pPolyLine->SetPoint(0, GetMinX(), GetMinY(), GetZOffset());
	pPolyLine->SetPoint(1, GetMaxX(), GetMinY(), GetZOffset());
	pPolyLine->SetPoint(2, GetMaxX(), GetMaxY(), GetZOffset());
	pPolyLine->SetPoint(3, GetMinX(), GetMaxY(), GetZOffset());
	pPolyLine->SetPoint(4, GetMinX(), GetMinY(), GetZOffset());
	return pPolyLine;
}
