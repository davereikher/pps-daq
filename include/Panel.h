#pragma once
#include <random>
#include <vector>
#include "Geometry.h"
#include "Cell.h"

#define NO_HIT -1
typedef std::vector<std::vector<Cell> > CellMatrix_t;

class Panel
{
public:
	Panel(int a_iIndex);
	Panel(){}

	int Captured(Geometry::Line3D a_track);
	void GenerateMatrix();
	std::vector<float> GetCentersVector(int a_iNumber, float a_fPitch, float a_fGlobalCenterPoint);
	void Draw();

private:
	int m_iIndex;
	float m_fCellEfficiency;
	CellMatrix_t m_cellMatrix;
	std::default_random_engine m_generator;
	std::uniform_real_distribution<float> m_distribution0To1;	
};
