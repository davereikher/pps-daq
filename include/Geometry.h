#pragma once

class Geometry
{
	class Point3D
	{
	public:
		Point3D(float a_fX, float a_fY, float a_fZ);
		float GetX();
		float GetY();
		float GetZ();
	private:
		float m_fX;
		float m_fY;
		float m_fZ;
	};

	class Line3D
	{
	public:
		Line(float a_fTheta, float a_fPhi, Point& a_point);
		Point3D GetPoint();
		float GetPhi();
		float GetTheta();
	private:
		float m_fPhi;
		float m_fTheta;
		Point3D m_point;
	};

	class HorizontalRectangle3D
	{
		HorizontalRectangle(float a_fLength, floart a_fWidth, float a_fXOffsetOfCenter, float a_fYOffsetOfCenter, float a_fZOffset);
		float GetZOffset();
		float GetMaxX();
		float GetMinX();
		float GetMaxY();
		float GetMinY();
	private:
		float m_fZOffset;
	};

	class Vector3D
	{
		float a_fX;
		float a_fY;
		float a_fZ;
	};

	Point LineWithHorizontalRectangleIntersection(HorizontalRectangle3D& a_rectangle, Line3D& a_line, Point3D& a_intersection);
	Point LineWithHorizontalPlaneIntersection(float a_fPlaneZValue, Line3D& a_line);
};
