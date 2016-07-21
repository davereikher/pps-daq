#include "DataPoint.h"

DataPoint::DataPoint()
{}

DataPoint::DataPoint(int a_iXDiscrete, int a_iYDiscrete, double a_fYStart, double a_fXConvertFactor, double a_fYConvertFactor, bool a_bExists):
m_fX(a_iXDiscrete * a_fXConvertFactor), m_fY(a_fYStart + a_iYDiscrete * a_fYConvertFactor), m_iXDiscrete(a_iXDiscrete), m_iYDiscrete(a_iYDiscrete), m_bExists(a_bExists)
{
	if(a_fYConvertFactor == 0)
	{
		m_fY = (float)a_iYDiscrete;
	}
	if(a_fXConvertFactor == 0)
	{
		m_fX = (float)a_iXDiscrete;
	}
	//printf("a_fYStart: %f, a_fYConvertFactor = %f\n", a_fYStart, a_fYConvertFactor);
}

double DataPoint::GetX()
{
	return m_fX;
}

double DataPoint::GetY()
{
	return m_fY;
}

bool DataPoint::Exists()
{
	return m_bExists;
}

int DataPoint::GetXDiscrete()
{
	return m_iXDiscrete;
}

int DataPoint::GetYDiscrete()
{
	return m_iYDiscrete;
}


