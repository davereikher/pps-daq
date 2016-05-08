#pragma once 
#include "Geometry.h"

class Scintillator: public HorizontalRectangle3D
{
public:
	Scintillator(float a_fEfficiency, HorizontalRectangle3D& a_polygon);
	bool Captured(Line3D& a_track);

private:
	float m_fEfficiency;
	std::default_random_engine m_generator;
	std::uniform_real_distribution<float> m_distribution0To1;
};