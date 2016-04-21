#include "Track.h"

Track::Track():
m_linearFitter(2)
{
}

void Track::AddPoint(float a_fX, float a_fZ, float a_fError)
{
	m_vXValues.push_back((Double_t)a_fX);
	m_vZValues.push_back((Double_t)a_fZ);
}

void Track::DoLinearRegression()
{
	if(m_bRegressionDone)
	{
		return;
	}
	m_bRegressionDone = true;
	m_linearFitter.SetFormula("pol1");
	m_linearFitter.AssignData(m_vXValues.size(), 2, m_vXValues.data(), m_vZValues.data(), m_vErrors.data());
	m_linearFitter.Eval();
}

float Track::GetChiSquarePerNDF()
{
	DoLinearRegression();
	Double_t fChiSquare = m_linearFitter.GetChisquare();
	return fChiSquare/m_linearFitter.GetNumberFreeParameters();
}

float Track::GetSlopeValue()
{
	DoLinearRegression();
	return m_linearFitter.GetParameter(1);
}

float Track::GetIntercept()
{
	DoLinearRegression();
	return m_linearFitter.GetParameter(2);
}

float Track::GetAngle()
{
	float angle = atan(GetSlopeValue());
	printf("angle = %f\n", angle);
	return angle;
}
