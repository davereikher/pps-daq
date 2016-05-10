#pragma once 
#include <random>
#include "TPolyLine3D.h"
#include "Geometry.h"

class BasicHorizontalRectangularDetectorElement: public Geometry::HorizontalRectangle3D
{
public:
	BasicHorizontalRectangularDetectorElement(float a_fEfficiency, HorizontalRectangle3D a_polygon);
	bool Captured(Geometry::Line3D& a_track);
	TPolyLine3D* GeneratePolyLine3D();

private:
	float m_fEfficiency;
	std::default_random_engine m_generator;
	std::uniform_real_distribution<float> m_distribution0To1;
};

