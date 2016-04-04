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
	
	return vRanges;
}

float Configuration::GetPulseThresholdVolts()
{
	return m_configuration["pulse-threshold-volts"].asFloat();
}

float Configuration::GetEdgeThresholdVolts()
{
	return m_configuration["edge-threshold-volts"].asFloat();
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

float Configuration::GetExpectedPulseWidthNs()
{
	return m_configuration["expected-pulse-width-ns"].asFloat();
}

float Configuration::GetMinEdgeSeparationNs()
{
	return m_configuration["min-edge-separation-ns"].asFloat();
}

float Configuration::GetMaxEdgeJitterNs()
{
	return m_configuration["max-edge-jitter-ns"].asFloat();
}

float Configuration::GetMaxAmplitudeJitterVolts()
{
	return m_configuration["max-amplitude-jitter-volts"].asFloat();
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
 
float Configuration::GetPulseStartThresholdVolts()
{
	return m_configuration["pulse-start-threshold-volts"].asFloat();
}

float Configuration::GetTriggerThresholdVolts()
{
	return m_configuration["trigger-threshold-volts"].asFloat();
}

