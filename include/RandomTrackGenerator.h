#pragma once
#include <random>
#include "Geometry.h"

class RandomTrackGenerator
{
public:
	RandomTrackGenerator(Geometry::HorizontalRectangle3D& a_pointDomainRectangle);
	void GenerateTracks(int a_iNumberOfTracks);

private:
	float GeneratePhiValue();
	float GenerateThetaValue();
	Geometry::Point3D GeneratePoint(Geometry::HorizontalRectangle3D& a_pointDomainRectangle);

private:
	std::default_random_engine m_generator;
	std::uniform_real_distribution<float> m_distributionPhi;
	std::uniform_real_distribution<float> m_distributionTheta;
	std::uniform_real_distribution<float> m_distribution0To1;
	std::uniform_real_distribution<float> m_distributionPointX;
	std::uniform_real_distribution<float> m_distributionPointY;
	
};
