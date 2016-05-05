#include <math.h>
#include "RandomTrackGenerator.h"

RandomTrackGenerator::RandomTrackGenerator(Geometry::HorizontalRectangle3D& a_pointDomainRectangle):
m_distributionPhi(0, 2*M_PI),
m_distributionTheta(0, M_PI/2),
m_distribution0To1(0,1),
m_distributionPointX(a_pointDomainRectangle.GetMinX(), a_pointDomainRectangle.GetMaxX()),
m_distributionPointY(a_pointDomainRectangle.GetMinY(), a_pointDomainRectangle.GetMaxY()),
{}

std::vector<Geometry::Line3D> RandomTrackGenerator::GenerateTracks(int a_iNumberOfTracks)
{
	std::vector<Geometry::Line3D> vTrackVector;
	for (int i = 0; i < a_iNumberOfTracks; i++)
	{
		Line3D line(GenerateThetaValue(), GeneratePhiValue(), GeneratePoint());
		vTrackVector.push_back(line);
	}
	return vTrackVector;
}

/**
Phi is uniformly distributed between 0 and 2pi
*/
float RandomTrackGenerator::GeneratePhiValue()
{
	return m_distributionPhi(m_generator);
}

/**
According to cos^2(theta) distribution. This is a simple implementation of rejection sampling:
	1. Pick a uniform random number x between 0 and pi/2
	2. Pick a uniform random number y between 0 and 1
	3. If y<=cos(x), return x
	4. Else, go to 1.
*/
float RandomTrackGenerator::GenerateThetaValue()
{
	float y = 0;
	float x = 0;
	float fCosSquared = 0;
	do
	{
		x = m_distributionTheta(m_generator);
		y = m_distribution0To1(m_generator);
		fCosSquared = cos(x);
		
	} while (y > fCosSquared * fCosSquared);

	return x;
}

Geometry::Point3D RandomTrackGenerator::GeneratePoint()
{
	Geometry::Point3D pt(m_distributionPointX(m_generator), m_distributionPointY(m_generator), 0);
}
