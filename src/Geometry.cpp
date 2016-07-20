#include <cmath>
#include <armadillo>
#include "stdio.h"
#include "Geometry.h"

using namespace arma;

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
	float fT = (a_fPlaneZValue - a_line.GetPoint().GetZ())/a_line.GetZComponent();
	float fX = a_line.GetPoint().GetX() + a_line.GetXComponent() * fT;
	float fY = a_line.GetPoint().GetY() + a_line.GetYComponent() * fT;
	return Point3D(fX, fY, a_fPlaneZValue);
}

float Geometry::GetPathLengthInHorizontalMedium(Geometry::Line3D& a_line, float a_fThickness)
{
	return a_fThickness/a_line.GetZComponent();
}

bool Geometry::PointExceedsBoundaries(Geometry::HorizontalRectangle3D& a_rectangle, Geometry::Point3D& a_point)
{
	if ( (a_point.GetX() <= a_rectangle.GetMaxX()) && (a_point.GetX() >= a_rectangle.GetMinX()) && (a_point.GetY() <= a_rectangle.GetMaxY()) && (a_point.GetY() >= a_rectangle.GetMinY()))
	{
		return false;
	}

	return true;
}

Geometry::Point3D Geometry::GetPointAtHorizontalPolarAngleAndDistanceFrom(Geometry::Point3D a_point, float a_fAngle, float a_fDistance)
{
	return Geometry::Point3D(a_point.GetX() + a_fDistance * sin(a_fAngle), a_point.GetY() + a_fDistance * cos(a_fAngle), a_point.GetZ());
}

Geometry::Point3D Geometry::GetPointAlongLineAtDistance(Geometry::Point3D a_origin, Geometry::Line3D a_line, float a_fDistance)
{
	return LineWithHorizontalPlaneIntersection(a_fDistance * a_line.GetZComponent() + a_origin.GetZ(), a_line);
}


float Geometry::DistanceBetweenTwoPoints(Geometry::Point3D a_point1, Geometry::Point3D a_point2)
{
	float fDeltaX = a_point1.GetX() - a_point2.GetX();
	float fDeltaY = a_point1.GetY() - a_point2.GetY();
	float fDeltaZ = a_point1.GetZ() - a_point2.GetZ();

	return sqrt(fDeltaX * fDeltaX + fDeltaY * fDeltaY + fDeltaZ * fDeltaZ);
}

Geometry::Line3D::Line3D(float a_fTheta, float a_fPhi, Geometry::Point3D a_point):
m_fPhi(a_fPhi), m_fTheta(a_fTheta), m_point(a_point)
{
	float fSinTheta = sin(a_fTheta);

	m_fX = fSinTheta * cos(a_fPhi);
	m_fY = fSinTheta * sin(a_fPhi);
	m_fZ = cos(a_fTheta);
}


Geometry::Line3D::Line3D(float a_fXComp, float a_fYComp, float a_fZComp, Geometry::Point3D a_point):
m_fX(a_fXComp),
m_fY(a_fYComp),
m_fZ(a_fZComp),
m_point(a_point)
{
	m_fPhi = atan(m_fY/m_fX);
	m_fTheta = acos(m_fZ);
}

void Geometry::Line3D::Rotate(Line3D a_rotationAxis, float a_fAngle)
{
//	printf("Rotating around Theta = %f, Phi = %f\n", a_rotationAxis.GetTheta(), a_rotationAxis.GetPhi());
	float fSinThetaRot = sin(a_rotationAxis.GetTheta());
	float fSinPhiRot = sin(a_rotationAxis.GetPhi());
	float fCosThetaRot = cos(a_rotationAxis.GetTheta());
	float fCosPhiRot = cos(a_rotationAxis.GetPhi());

	float fSinTheta = sin(m_fTheta);
	float fCosTheta = cos(m_fTheta);
	float fSinPhi = sin(m_fPhi);
	float fCosPhi = cos(m_fPhi);
	float fSinAngle = sin(a_fAngle);
	float fSinAngleHalf = sin(a_fAngle/2);
	vec lv;
	lv << fSinTheta * fCosPhi << fSinTheta * fSinPhi << fCosTheta;
	vec rot;
	rot << fSinThetaRot * fCosPhiRot << fSinThetaRot * fSinPhiRot << fCosThetaRot;
	

	mat W(3, 3);
	W 	<<	0 		<<	-1*rot(2)	<< 	rot(1)		<<	endr
		<<	rot(2)		<<	0		<<	-1 * rot(0)	<<	endr
		<<	-1 * rot(1)	<<	rot(0)		<<	0		<<	endr;
	
	
	mat rodriguez = eye<mat>(3, 3) + W*fSinAngle + W*W*2*fSinAngleHalf * fSinAngleHalf;
	vec lvPrime = rodriguez * lv;
	
//	printf("theta=%f, phi=%f ", m_fTheta * (180/M_PI), m_fPhi * (180/M_PI));
	m_fTheta = acos(lvPrime(2));
	m_fPhi = atan(lvPrime(1)/lvPrime(0));
	m_fX = lvPrime(0);
	m_fY = lvPrime(1);
	m_fZ = lvPrime(2);
//	printf("rotated to theta=%f, phi=%f\n", m_fTheta * (180/M_PI), m_fPhi * (180/M_PI));
/*	if (m_fPhi < 0)
	{
		m_fPhi = 2*M_PI - m_fPhi;
	}*/
//	m_fTheta = atan(sqrt((lvPrime(0) * lvPrime(0) + lvPrime(1) * lvPrime(1))/(lvPrime(2) * lvPrime(2))));
}

