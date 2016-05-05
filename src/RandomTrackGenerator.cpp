#include <math.h>
#include "RandomTrackGenerator.h"

RandomTrackGenerator::RandomTrackGenerator():
m_distributionPhi(0, 2*M_PI),
m_distributionTheta(0, M_PI/2),
m_distribution0To1(0,1)
{}

void RandomTrackGenerator::GenerateTracks(int a_iNumberOfTracks, Geometry::HorizontalRectangle3D& a_pointDomainRectangle)
{
	
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
	float x = 0;1
	do
	{
		x = m_distributionTheta(m_generator);
		y = m_distribution0To1(m_generator);
	} while (y > cos(x));

	return x;
}
