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

	class Line3D
	{
	public:
		Line3D(float a_fTheta, float a_fPhi, Geometry::Point3D a_point):
		m_fPhi(a_fPhi), m_fTheta(a_fTheta), m_point(a_point){}
		Line3D(){}
		
		Point3D GetPoint() {return m_point;}
		float GetPhi()	{return  m_fPhi;}
		float GetTheta() {return m_fTheta;}
	private:
		float m_fPhi;
		float m_fTheta;
		Point3D m_point;
	};

	class HorizontalRectangle3D
	{
	public:
		HorizontalRectangle3D(float a_fLengthX, float a_fLengthY, float a_fXOffsetOfCenter, float a_fYOffsetOfCenter, float a_fZOffset):
		m_fLengthX(a_fLengthX), m_fLengthY(a_fLengthY), m_fXOffsetOfCenter(a_fXOffsetOfCenter), m_fYOffsetOfCenter(a_fYOffsetOfCenter), m_fZOffset(a_fZOffset) {}
		HorizontalRectangle3D(){}
		float GetZOffset() {return m_fZOffset;}
		float GetMaxX() {return m_fXOffsetOfCenter + m_fLengthX/2;}
		float GetMinX() {return m_fXOffsetOfCenter - m_fLengthX/2;}
		float GetMaxY() {return m_fYOffsetOfCenter + m_fLengthY/2;}
		float GetMinY() {return m_fYOffsetOfCenter - m_fLengthY/2;}
	private:
		float m_fLengthX;
		float m_fLengthY;
		float m_fXOffsetOfCenter;
		float m_fYOffsetOfCenter;
		float m_fZOffset;
	};
	
	class Vector3D
	{
		float a_fX;
		float a_fY;
		float a_fZ;
	};

	static bool LineWithHorizontalRectangleIntersection(HorizontalRectangle3D a_rectangle, Line3D a_line, Point3D& a_intersection);
	static Point3D LineWithHorizontalPlaneIntersection(float a_fPlaneZValue, Line3D& a_line);
};
