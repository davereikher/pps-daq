#include <vector>
#include <utility>
#include "TLinearFitter.h"

class Track
{
public:
	Track();
	void AddPoint(float a_fX, float a_fZ, float a_fError=0);
	float GetChiSquaredPerNDF();
	float GetSlopeValue();
	float GetIntercept();
	float GetAngle();
	int GetNumberOfPoints();

private:
	void DoLinearRegression();

private:
	std::vector<Double_t> m_vXValues;
	std::vector<Double_t> m_vZValues;
	std::vector<Double_t> m_vErrors;
	bool m_bRegressionDone;
	TLinearFitter m_linearFitter;
	float m_fSlope;
	float m_fIntercept;
};
