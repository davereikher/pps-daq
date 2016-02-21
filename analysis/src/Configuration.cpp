#include <fstream>
#include <iostream>
#include "Configuration.h"

Json::Value Configuration::m_configuration;

void Configuration::LoadConfiguration(const char* a_pFilename) {
	//TODO: check exceptions
	printf ("File name: %s\n", a_pFilename);
	std::ifstream in(a_pFilename, std::ifstream::binary);
	std::cout << "BEFORE" << std::endl;
	Json::Reader reader;
	int successful = reader.parse(in, m_configuration, false );

	if(!successful)
	{
		//TODO: exception here
		std::cout << "Could not read json. Error: " << reader.getFormattedErrorMessages() << std::endl;
	}

	/*	Json::Reader reader;
		bool parsedSuccess = reader.parse(json_example,
		root,
		false);*/

	/*	if(not parsedSuccess)
		{
	// Report failures and their locations
	// in the document.
	cout<<"Failed to parse JSON"<<endl
	<<reader.getFormatedErrorMessages()
	<<endl;
	return 1;

	}*/
	std::cout << "OK" <<std::endl;
	in.close();
}

std::map<std::string, std::vector<int> > Configuration::GetRanges()
{
	std::map<std::string, std::vector<int> > vRanges;

	Json::Value ranges = m_configuration["panel-ranges"];
	Json::Value::Members panelKeys = ranges.getMemberNames();

	for (auto& panelKey: panelKeys) 
	{
		std::vector<int> singlePanelRange;
		for (auto& channelIt: ranges[panelKey])
		{
			singlePanelRange.push_back(channelIt.asInt());
		}
		vRanges[panelKey]= singlePanelRange;
	}

	for (int i = 0; i < vRanges["A"].size(); i++)
	{
		printf("Panel A: %d\n", vRanges["A"][i]);
	}

	for (int i = 0; i < vRanges["B"].size(); i++)
	{
		printf("Panel B: %d\n", vRanges["B"][i]);
	}

	
	return vRanges;
}

