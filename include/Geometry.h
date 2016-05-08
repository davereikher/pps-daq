#pragma once

class Geometry
{
public:
	class Point3D
	{
	public:
		Point3D(float a_fX, float a_fY, float a_fZ):m_fX(a_fX),m_fY(a_fY),m_fZ(a_fZ){}
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
		Line3D(float a_fTheta, float a_fPhi, Point3D& a_point);
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
		HorizontalRectangle3D(float a_fLength, float a_fWidth, float a_fXOffsetOfCenter, float a_fYOffsetOfCenter, float a_fZOffset);
		float GetZOffset() {return m_fZOffset;}
		float GetMaxX() {return 0;}
		float GetMinX() {return 0;}
		float GetMaxY() {return 0;}
		float GetMinY() {return 0;}
	private:
		float m_fZOffset;
	};
	
	class Vector3D
	{
		float a_fX;
		float a_fY;
		float a_fZ;
	};

	static bool LineWithHorizontalRectangleIntersection(HorizontalRectangle3D& a_rectangle, Line3D& a_line, Point3D& a_intersection);
	static Point3D LineWithHorizontalPlaneIntersection(float a_fPlaneZValue, Line3D& a_line);
};
