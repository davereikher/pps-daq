#pragma once 
#include <random>
#include "TPolyLine3D.h"
#include "Geometry.h"
#include "BasicHorizontalRectangularDetectorElement.h"

class Scintillator: public BasicHorizontalRectangularDetectorElement
{
public:
	Scintillator(float a_fEfficiency, HorizontalRectangle3D a_polygon);
};
