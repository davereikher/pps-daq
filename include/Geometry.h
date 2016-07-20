#pragma once

class Geometry
{
public:
	class Point3D
	{
	public:
		Point3D(float a_fX, float a_fY, float a_fZ):m_fX(a_fX),m_fY(a_fY),m_fZ(a_fZ){}
		Point3D(){}
		float GetX() {return m_fX;}
		float GetY() {return m_fY;}
		float GetZ() {return m_fZ;}
	private:
		float m_fX;
		float m_fY;
		float m_fZ;
	};

	class Point2D: public Point3D
	{
	public:
		Point2D(float a_fX, float a_fZ):Point3D(a_fX, 0, a_fZ){}
		Point2D(){}
	};

	class Line3D
	{
	public:
		Line3D(float a_fTheta, float a_fPhi, Geometry::Point3D a_point);
		Line3D(float a_fXComp, float a_fYComp, float a_fZComp, Geometry::Point3D a_point);
		Line3D(){}
		void Rotate(Line3D a_rotationAxis, float a_fAngle);
		
		Point3D GetPoint() {return m_point;}
		float GetPhi()	{return  m_fPhi;}
		float GetTheta() {return m_fTheta;}
		float GetXComponent(){return m_fX;}	
		float GetYComponent(){return m_fY;}
		float GetZComponent(){return m_fZ;}
	private:
		float m_fPhi;
		float m_fTheta;
		float m_fX;
		float m_fY;
		float m_fZ;
		Point3D m_point;
	};

	class HorizontalRectangle3D
	{
	public:
		HorizontalRectangle3D(float a_fLengthX, float a_fLengthY, Point3D a_centeriPoint):
		m_fLengthX(a_fLengthX), m_fLengthY(a_fLengthY), m_centerPoint(a_centeriPoint) {}
		HorizontalRectangle3D(){}
		float GetZOffset() {return m_centerPoint.GetZ();}
		float GetMaxX() {return m_centerPoint.GetX() + m_fLengthX/2;}
		float GetMinX() {return m_centerPoint.GetX() - m_fLengthX/2;}
		float GetMaxY() {return m_centerPoint.GetY() + m_fLengthY/2;}
		float GetMinY() {return m_centerPoint.GetY() - m_fLengthY/2;}
		Point3D GetCenterPoint() {return m_centerPoint;}
	private:
		float m_fLengthX;
		float m_fLengthY;
		Point3D m_centerPoint;
	};
/*
	class Angle
	{
		Angle(float a_fDeltaX, float a_fDeltaZ);
		bool operator==(float a_fAngle) {return m_fAngle == a_fAngle;}
	public:
		float m_fAngle;
	};
	
	class Vector3D
	{
		float a_fX;
		float a_fY;
		float a_fZ;
	};
*/
	static bool LineWithHorizontalRectangleIntersection(HorizontalRectangle3D a_rectangle, Line3D a_line, Point3D& a_intersection);
	static Point3D LineWithHorizontalPlaneIntersection(float a_fPlaneZValue, Line3D& a_line);
	static float GetPathLengthInHorizontalMedium(Geometry::Line3D& a_line, float a_fThickness);
	static bool PointExceedsBoundaries(Geometry::HorizontalRectangle3D& a_rectange, Geometry::Point3D& a_point);
	static Geometry::Point3D GetPointAtHorizontalPolarAngleAndDistanceFrom(Geometry::Point3D a_point, float a_fAngle, float a_fDistance);
	static Geometry::Point3D GetPointAlongLineAtDistance(Geometry::Point3D a_origin, Geometry::Line3D a_line, float a_fDistance);
	static float DistanceBetweenTwoPoints(Geometry::Point3D a_point1, Geometry::Point3D a_point2);

};
