#include "Scintillator.h"

Scintillator::Scintillator(float a_fEfficiency, HorizontalRectangle3D& a_polygon):
	HorizontalRectangle3D(a_polygon),
	m_fEfficiency(a_fEfficiency)
{}

bool Scintillator::Captured(Line3D& a_track)
{
	Point3D intersection;
	if (Geometry::LineWithHorizontalRectangleIntersection(*this, a_track, intersection))
	{
		if (m_distribution0To1(m_generator) <= m_fEfficiency)
		{
			return true;
		}
	}
}
