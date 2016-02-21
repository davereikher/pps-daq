#pragma once

#include "json/json.h"

class Configuration
{
public:
	void LoadConfiguration(const char* a_pFilename);
	std::map<std::string, std::vector<int> > GetRanges();

private:
	static Json::Value m_configuration;
};
