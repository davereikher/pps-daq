#include "Track.h"

Track::Track():
m_bRegressionDone(false),
m_linearFitter(1, "1++x"),
m_fSlope(0),
m_fIntercept(0)
{
/*	m_vXValues.push_back(0);
	m_vZValues.push_back(0);*/
}

void Track::AddPoint(float a_fX, float a_fZ, float a_fError)
{
	m_vXValues.push_back((Double_t)a_fX);
	m_vZValues.push_back((Double_t)a_fZ);
	printf("Added point %f, %f\n", a_fX, a_fZ);
}

int Track::GetNumberOfPoints()
{
	return m_vXValues.size();
}

void Track::DoLinearRegression()
{
	if(m_bRegressionDone)
	{
		return;
	}
	m_bRegressionDone = true;
	printf("blah\n");

	if(m_vXValues.size() == 2)
	{
		m_fSlope = (m_vXValues[0] - m_vXValues[1])/(m_vZValues[0] - m_vZValues[1]);
	}
	else
	{
	//	m_linearFitter.SetFormula("1++x");
		m_linearFitter.AssignData(m_vXValues.size(), 1, m_vZValues.data(), m_vXValues.data(), m_vErrors.data());
		m_linearFitter.Eval();
		m_fSlope = m_linearFitter.GetParameter(1);
		m_fIntercept = m_linearFitter.GetParameter(0);
	}
	printf("moo\n");
}

float Track::GetChiSquaredPerNDF()
{
	DoLinearRegression();
	Double_t fChiSquare = m_linearFitter.GetChisquare();
	float fResult = fChiSquare/m_linearFitter.GetNumberFreeParameters();
	return fResult;
}

float Track::GetSlopeValue()
{
	DoLinearRegression();
	return m_fSlope;
}

float Track::GetIntercept()
{
//	DoLinearRegression();
	return m_fIntercept;
}

float Track::GetAngle()
{
	float fSlope = GetSlopeValue();
	float angle = atan(fSlope);

	printf("slope=%f, angle = %f\n", fSlope, angle);
	return angle;
}

