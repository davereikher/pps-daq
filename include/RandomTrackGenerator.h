#pragma once
#include "Geometry.h"

class RandomTrackGenerator
{
public:
	void GenerateTracks(int a_iNumberOfTracks, Geometry::HorizontalRectangle3D& a_pointDomainRectangle);
private:
	float GeneratePhiValue();
	float GenerateThetaValue();
	Point GeneratePoint(Geometry::HorizontalRectangle3D& a_pointDomainRectangle);
};
