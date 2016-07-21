#pragma once
class DataPoint
{
public:
	DataPoint(int a_iXDiscrete, int a_iYDiscrete, double a_fYStart, double a_fXConvertFactor = 1, double a_fYConvertFactor = 1, bool a_bExists = true);
	DataPoint();
	double GetX();
	double GetY();
	int GetXDiscrete();
	int GetYDiscrete();
	bool Exists();

private:
	double m_fX;
	double m_fY;
	int m_iXDiscrete;
	int m_iYDiscrete;
	bool m_bExists;
};


