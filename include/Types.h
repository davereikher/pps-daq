#pragma once
#include <map>
#include <vector>
#include "DataPoint.h"
#define EDGE_THRES_INDEX 0
#define MIN_PULSE_INDEX 1

typedef std::map<std::string, std::vector<int> > Range_t;
typedef std::vector<std::vector<float> > Channels_t;
typedef std::map<std::string, int> HitMap_t;
typedef std::vector<std::tuple<DataPoint, DataPoint> > SignalVector_t;

