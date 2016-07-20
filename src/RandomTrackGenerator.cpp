#include <math.h>
#include "RandomTrackGenerator.h"
#include "Configuration.h"

RandomTrackGenerator::RandomTrackGenerator(Geometry::HorizontalRectangle3D a_pointDomainRectangle):
m_distributionPhi(0, 2*M_PI),
m_distributionTheta(0, M_PI/2),
m_distribution0To1(0,1),
m_distributionPointX(a_pointDomainRectangle.GetMinX(), a_pointDomainRectangle.GetMaxX()),
m_distributionPointY(a_pointDomainRectangle.GetMinY(), a_pointDomainRectangle.GetMaxY()),
m_fPointZOffset(a_pointDomainRectangle.GetZOffset())
{
	std::random_device r;
	std::seed_seq seed{r(), r(), r(), r(), r(), r(), r()};
	m_generator.seed(seed);
//	printf("MaxX: %f, MinX: %f, MaxY: %f, MinY: %f\n", a_pointDomainRectangle.GetMaxX(), a_pointDomainRectangle.GetMinX(), a_pointDomainRectangle.GetMaxY(), a_pointDomainRectangle.GetMinY());
}

Geometry::Line3D RandomTrackGenerator::GenerateTrack()
{
//	printf("Generated...");
	Geometry::Line3D line(GenerateThetaValue(), GeneratePhiValue(), GeneratePoint());
	float fRotationAngle = Configuration::Instance().GetGlobalRotationAngle();
	if (fRotationAngle != 0)
	{
		Configuration& config = Configuration::Instance();
//		printf("Rotating: theta = %f, phi = %f\n", config.GetGlobalRotationAxisTheta(), config.GetGlobalRotationAxisPhi());
		line.Rotate(Geometry::Line3D(config.GetGlobalRotationAxisTheta(), config.GetGlobalRotationAxisPhi(), line.GetPoint()), fRotationAngle);
	}
	return line;
}

/**
Phi is uniformly distributed between 0 and 2pi
*/
float RandomTrackGenerator::GeneratePhiValue()
{
//TODO: return this!!!
	return m_distributionPhi(m_generator);

//return 0;
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
	
//	printf("Theta: %f\n", x);

//TODO:  RETURN THIS!
	return x;

//TODO: REMOVE THIS!
//	return 0;
}

Geometry::Point3D RandomTrackGenerator::GeneratePoint()
{
	Geometry::Point3D pt(m_distributionPointX(m_generator), m_distributionPointY(m_generator), m_fPointZOffset);
//	printf("Random Point is at (%f, %f, %f)\n", pt.GetX(), pt.GetY(), pt.GetZ());
	return pt;
}
