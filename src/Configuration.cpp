#include <fstream>
#include <sstream>
#include <iostream>
#include "Configuration.h"

Configuration Configuration::m_instance;

Configuration::Configuration():
m_bInitialized(false)
{
}

Configuration& Configuration::Instance()
{
	return m_instance;
}

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
	m_bInitialized = true;
}


std::map<std::string, std::vector<int> > Configuration::GetRanges()
{
	std::map<std::string, std::vector<int> > vRanges;

	Json::Value ranges = m_configuration["panels"];
	Json::Value::Members panelKeys = ranges.getMemberNames();

	for (auto& panelKey: panelKeys) 
	{
		std::vector<int> singlePanelRange;
		//printf("\npanel key = %s\n", panelKey.c_str());
		for (auto& channelIt: ranges[panelKey]["range"])
		{
			//printf("%d ", channelIt.asInt());
			singlePanelRange.push_back(channelIt.asInt());
		}
		vRanges[panelKey]= singlePanelRange;
	}
	
	return vRanges;
}

float Configuration::GetPulseThresholdVolts(std::string a_sPanelName)
{
	return m_configuration["panels"][a_sPanelName]["pulse-threshold-volts"].asFloat();
}

float Configuration::GetEdgeThresholdVolts(std::string a_sPanelName)
{
	//printf("\nEDGE THRESHOLD VOLTS FOR %s is %f\n", a_sPanelName.c_str(), m_configuration["panels"][a_sPanelName]["edge-threshold-volts"].asFloat());
	return m_configuration["panels"][a_sPanelName]["edge-threshold-volts"].asFloat();
}

float Configuration::GetVoltMin()
{
	return m_configuration["volt-min"].asFloat();
}

float Configuration::GetVoltMax()
{
	return m_configuration["volt-max"].asFloat();
}

int Configuration::GetDigitizerResolution()
{
	return m_configuration["digitizer-resolution-bits"].asInt();
}

float Configuration::GetExpectedPulseWidthNs(std::string a_sPanelName)
{
	return m_configuration["panels"][a_sPanelName]["expected-pulse-width-ns"].asFloat();
}

float Configuration::GetMinEdgeSeparationNs(std::string a_sPanelName)
{
	return m_configuration["panels"][a_sPanelName]["min-edge-separation-ns"].asFloat();
}

float Configuration::GetMaxEdgeJitterNs(std::string a_sPanelName)
{
	return m_configuration["panels"][a_sPanelName]["max-edge-jitter-ns"].asFloat();
}

float Configuration::GetMaxAmplitudeJitterVolts(std::string a_sPanelName)
{
	return m_configuration["panels"][a_sPanelName]["max-amplitude-jitter-volts"].asFloat();
}

float Configuration::GetSamplingFreqGHz()
{
	return m_configuration["sampling-freq-ghz"].asFloat();
}

int Configuration::GetNumberOfEventsToDrawAfter()
{
	return m_configuration["number-of-events-to-draw-after"].asInt();
}

seconds Configuration::GetTriggerRateAveragingDurationSecs()
{
	return seconds(m_configuration["trigger-rate-averaging-duration-seconds"].asInt());
}

float Configuration::GetIdleFluctuationsAmplitude()
{
	return m_configuration["idle-fluctuation-amplitude-volts"].asFloat();
}

float Configuration::GetIdleLineDurationFraction()
{
	return m_configuration["idle-line-duration-fraction"].asFloat();
}

std::string Configuration::GetDump()
{
	std::stringstream stream;
	stream << m_configuration;
	return std::string("\nConfiguration Dump \n-------------------------------------------\n") + stream.str() + "\n\n";
}

bool Configuration::ShouldNormalizeChannels()
{
	return m_configuration["normalize-channels"].asBool();
}
 
float Configuration::GetPulseStartThresholdVolts(std::string a_sPanelName)
{
	return m_configuration["panels"][a_sPanelName]["pulse-start-threshold-volts"].asFloat();
}

float Configuration::GetTriggerThresholdVolts()
{
	return m_configuration["trigger-threshold-volts"].asFloat();
}

