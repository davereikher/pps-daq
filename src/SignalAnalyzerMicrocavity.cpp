#include <chrono>
#include <unistd.h>
#include "TSystem.h"
#include "SignalAnalyzerMicrocavity.h"
#include "TApplication.h"
#include "Configuration.h"
#include "Logger.h"
#include "keyb.h"

//using std;
using namespace std::chrono;

/**
Constructor. 
*/
SignalAnalyzerMicrocavity::SignalAnalyzerMicrocavity():
m_pTriggerTimingMonitor(new TriggerTimingMonitor(milliseconds(Configuration::Instance().GetTriggerRateAveragingDurationSecs())))
{}

// MOST IMPORTANT METHOD! This is where you add your code
void SignalAnalyzerMicrocavity::DoAnalysis(nanoseconds a_timeStamp, Channels_t& a_vChannels)
{
	/* So a pulse in the TRG_IN input of the digitizer caused the acquisition (snapshot of all 32 channels + 1 channels representing the tr0 input. More on this below) to be taken by the digitizer, stored to it's 
			internal memory, transferred to this PC via USB and eventually this function was called. The acquired data from all channels is stored in a_vChannels, and this is done every time a trigger is generated.*/


	///////////////////////////////////////////////////////////////--MOST BASIC ANALYSIS--////////////////////////////////////////////////////////////////////////////////////////////////////////

	/* First, if we're interested only in the timing of the arrival, we don't need to analyze the data in a_vChannels, but just look at a_timeStamp. This is good, for example, for monitoring the trigger rate, for which we don't
		need the raw data, but only the time stamp, so we pass it (a_timeStamp) to the trigger monitor so that it could plot a trigger timing distribution or any other plots you can think of.
		Notice that a flag AnalysisFlagsMicrocavity::EtriggerTimingMonitor, which is defined in SignalAnalysisMicrocavity.h must be set by the method SetFlags() of this class to tell this method to analyze the trigger timing.
		Those flags are necessary to be able to keep the amount of plots on the screen to a necessary minimum, so you can turn on and off the monitors you implement by calling SetFlags() with the corresponding flags.
	*/
	if (m_iFlags & AnalysisFlagsMicrocavity::ETriggerTimingMonitor)
	{
		//The method GotTrigger in the TriggerTimingMonitor class does everything related to monitoring the trigger rate - it accepts the time stamp and then does all necessary calculations to generate the resulting plots.
		//The best way to keep the code clean is to make a separate class, like the TriggerTimingMonitor, for every separate thing you wish to monitor and call the main method of that class from here, just like this:
		m_pTriggerTimingMonitor->GotTrigger(a_timeStamp);
	}

	

	if(a_vChannels.size() == 1)
	{
		if(a_vChannels[0].size() == 1)
		{
			/* Event is empty, meaning that there is no data, so there's nothing to do with it anymore, so we return from the function. 
				This (empty data) doesn't happen during acquisition directly from the digitizer, because during acquisition, 
				we always get all samples from all channels. This can, however, happen during analysis, when the data is read from a root file. 
				During acquisition, each acquired event (every time a trigger is generated) is saved into a root file. If an event doesn't have any interesting signals in any one of the channels (if there are no samples 
				in any one of the channels that pass a certain voltage threshold defined in the configuration (in the field idle-fluctuation-amplitude-volts)), it is saved as an empty event to save disk space. 
				In case you're wondering, we're saving those events to perform off-line trigger timing analysis, because we still need the time stamps for that.
			*/
			return;
		}
	}
		

	
	///////////////////////////////////////////////////////////////--ANALYSIS OF RAW DATA--////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/* If we reached this spot, then a_vChannels contains legitimate data in the form of vector<vector<float> > (this is the type Channels_t, which is defined in Types.h). If you are not familiar with STL (the C++ Standard Library),
		the type vector<T> is a template class implementing an array of type T. For example, vector<float> is an array where each item is of type float. The type vector<vector<float> > is an array where each item is
		of type vector<float>. The cool thing about the vector class (and STL data structures in general) is that you don't need to concern yourself with things like memory allocation. The vector class does everything for you, and that is the main reason for using STL here.
		Anyway, the type Channels_t, which is a vector<vector<float> >, is a two-dimensional array (look at it as a table):

				|	   \	samples	|	0	|	1	|	2	|	...	|	1022	|	1023	|
				| channels  \		|
				=========================================================================================================================
				|	0		|
				-------------------------------------------------------------------------------------------------------------------------
				|	1		|	
				-------------------------------------------------------------------------------------------------------------------------
				|	2		|
				-------------------------------------------------------------------------------------------------------------------------
					.		
					.		
					.
				-------------------------------------------------------------------------------------------------------------------------
				|	32		|
				-------------------------------------------------------------------------------------------------------------------------
				|	33 (TR0)	|
				-------------------------------------------------------------------------------------------------------------------------

		So each row in this table represents a channel and is an array of all samples (1024 in total) acquired from that channel.
		There are 32 channels in the CAEN V1742 digitizer, so the first 32 rows correspond to those 32 channels, in order (channel 0 -> row 0, channel 1-> row 1, etc). For each row, the columns are the samples.
		Look at each sample as a point in a time-series of voltage vs. time. Only here, the voltage is a 12-bit integer value (yes, I know, vector<vector<float> > is a bit misleading. It's actually supposed to be
		vector<vector<unsigned long int> >, but it works with the float as well, so I left it like that), which needs to be converted to voltage and the time is an index taking values between 0 and 1023 which needs to be
		converted to seconds. You can do those conversions yourself knowing the following specifications:
			maximum voltage : 0.5V
			minimum voltage : -0.5V
			voltage resolution: 12 bits. For 12 bits, the maximum value is binary 111111111111 which is decimal 4095, so the resolution is (max. voltage - min. voltage)/bits_max = (0.5-(-0.5))/4095 = 0.244 mV, 
					so to convert a sample to it's voltage value you need to multiply the sample value by 0.244 mV and add -0.5V (sample value 0 corresponds to -0.5V)
			Sampling rate: 2.5 GHz (2.5 billion samples per second), so the samples are 0.4 ns away from each other. This, by the way, gives a total acquisition window of 1024 * 0.4ns ~ 400 ns.

		Some words about the last row (33): In order to perform timing analysis of signals from a detector (for example, analyse the intervals between the arrival time of the pulse from the detector and the trigger), 
			or any kind of analysis making it necessary to know the trigger arrival time with high precision, you must feed the trigger into the input TR0, as well as TRG_IN. The input TRG_IN just tells the 
			digitizer to take a snapshot whenever there is a trigger, but does not record the waveform of the trigger and is very imprecise. The TR0 input is an additional channel that is used to precisely 
			record the waveform of the trigger along with all the other channels. This waveform appears, in the same way as the other channels (1024 samples, 12 bit resolution, blah blah blah) in channel number 33.

		Back to all the conversions described above, luckily, you already have some useful functions to do all the convertions and the analyses of pulses for you, so you can use them instead. Let's look at them:
	*/					

	
	///////////////////////////////////////////////////////////////--ANALYSIS WITH THE HELP OF AVAILABLE FUNCTIONS - CORRECTING DC OFFSETS--/////////////////////////////////////////////////////////////////////////////////
	
	Channels_t vNormalizedChannels;
	vNormalizedChannels = NormalizeChannels(a_vChannels);
	/* When the data arrives, the differences in the resistances of the attenuators used in the inputs to the channels, causes a slight DC offset between channels. To remove that offset "normalization" is performed (this should have been called 'equalization' or something like that, since it's just adding DC offsets). This 
		involves going over all channels in a_vChannels, finding the DC offset from the actual 0 value of those channels and subtracting this DC offset from all samples in that channel. The method that implements this is
		NormalizeChannels(). vNormalizedChannels contains the same data, but without those DC offsets.

		You can do whatever you want with vNormalizedChannels here ... 

	*/

	
	///////////////////////////////////////////////////////////////--ANALYSIS WITH THE HELP OF AVAILABLE FUNCTIONS - DETECTING PULSES--////////////////////////////////////////////////////////////////////////////////////////

	/* The next nice-to-have thing is if we could go over each channel and detect if there is a pulse in that channel or not and if there is, where the leading edge is located and what is the extremum of the pulse. This is
		easily done with the provided method that is declared as such:
			
			std::tuple<DataPoint, DataPoint> SignalAnalyzer::FindLeadingEdgeAndPulseExtremum(std::vector<float>& a_samplesVector)
	
		As you can see, the method accepts a vector<float> (the 'std::' is called a namespace and just tells the compiler where to look for the class vector. C++ stuff, don't mind it...), which is exactly the array of 
		samples representing a single channel. The method goes over the samples and detects pulses according to thresholds defined in the configuration file. An explanation of the pulse detection process with all relative
		thresholds is described in 
				https://drive.google.com/open?id=0B3o6aorngnTnTEtKSzlldjVkMTg
		in chapter 7.4 - Acquisition and Analysis Software.
		The method returns a tuple of two DataPoints. A std::tuple is another useful STL data structure representing a finite and fixed number of values. The DataPoint class is implemented	
		in DataPoint.cpp and represents a single point in the time-series which is the signal waveform in any channel. The DataPoint class implements the following methods:
			--- Methods of DataPoint ---
				* double GetX() - get the time of the point, in nanoseconds
				* double GetY() - get the voltage of the point, in volts
				* int GetXDiscrete() - get the index of the point (0..1023)
				* int GetYDiscrete() - get the raw voltage value, as read from the digitizer (0..4095)
				* bool Exists() - whether the point exists or not (for example, if there is no pulse in a channel, the above method returns DataPoints for which this 
					method returns false).
		The first DataPoint in the tuple is the location of the leading edge (the point at which the waveform crosses the threshold defined in the configuration as edge-threshold-volts.
			See above reference for more details) and the second DataPoint is the location of the extremum of the pulse.
			________________________________________________________________________________________________________________________________________________________________________
			|NOTE: the methods described here only detect PRIMARY PULSES for each channel. These are the first pulses detected within a single acquisition window. For each channel,|
			|	pulses following the primary pulse on that channel will not be reported and will be ignored. If you want to analyze secondary pulses, you will need to write	|
			|	your own function, using the raw data, as described in the previous sections.											|
			------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		So, in order to detect primary pulses for all channels, you would do something like this:
		
		for (int i = 0; i < 32; i++)
		{
			//Go over the first 32 channels. We'll treat channel 33 (TR0) later.
			std::tuple<DataPoint, DataPoint> dp = FindLeadingEdgeAndPulseExtremum(vNormalizedChannels[i]);
			
			//Do something with the points, like pass them to a monitor.
			//Use the following syntax to access elements of tuples:
			//To get the time value in ns of the first element (leading edge):
			//std::get<EDGE_THRES_INDEX>(dp).GetX();

			//To get the voltage value in volts of the first element (leading edge):	
			//std::get<EDGE_THRES_INDEX>(dp).GetY();
			
			//To get the second element (pulse extremum), it's the same but instead of EDGE_THRES_INDEX use MIN_PULSE_INDEX. These constants are defined in Types.h.
		}


	Another interesting function to have would be one that looks at a bunch of channels in a single acquisition window, and determines which single channel was the first to have a pulse.
	This function is declared as 

		void SignalAnalyzer::FindOriginalPulseInChannelRange(Channels_t& a_vAllChannels, std::string a_sPanelName, std::vector<int>& a_vRange)
	
	Here, a_vAllChannels is the data of the familiar type Channels_t (the 32 channels + 1 TR0 channel) and a_vRange is a vector of channels in this bunch. The a_sPanelName is the name given 
	to the bunch of channels in the configuration. Notice that the function does not return a value. That is because the result is stored in the data member m_markers of this class.
	This data member is an instance of the class AnalysisMarkers defined in SignalAnalyzer.h. The result is stored in the data member m_vChannelsWithPulse of that class, which is a vector<int>.
	The result is a vector of numbers and not a single number, because sometimes, the thresholds defined in the configuration are not enough to single out one channel with a 'first pulse'. This
	happens when, for example, there are several 'first pulses' very close to each other, so we cannot say which one of the is truly the first. In such cases, all channels which might contain
	the first pulse are reported and they are stored in that vector. Now an example. Suppose you want to find which one of channels {1, 4, 7, 10, 28} fired first (had the first pulse).
	For that, you place the following under the "panels" key in the configuration file (remember the file is in JSON format. You can check the validity of the file in on-line JSON validators, 
	like jsonlint.com by copying and pasting the contents of the file. It's best to do it when there are large changes to the configuration file, to avoid errors). 
		"A": {

			"channels": [1, 4, 7, 10, 28],
			"lines": [1, 4, 7, 10, 28],
			"pulse-threshold-volts": -0.13,
			"edge-threshold-volts": -0.11,
			"pulse-start-amplitude-volts": 0.02,
			"expected-pulse-width-ns": 3,
			"min-edge-separation-ns": 3,
			"max-edge-jitter-ns": 2,
			"max-amplitude-jitter-volts": 0.1,
			"index": 0
		}

	Then, you use the following code
		
		//Call the function. Use an auxiliary method in the configuration class to get the ranges of panel "A"
		FindOriginalPulseInChannelRange(a_vChannels, "A", Configuration::Instance().GetRanges()["A"])
		
		//Do something with the result;
		DoSomething(m_markers.m_vChannelsWithPulse)
	
		
	///////////////////////////////////////////////////////////////--ANALYSIS WITH THE HELP OF AVAILABLE FUNCTIONS - DETECTING PULSES--////////////////////////////////////////////////////////////////////////////////////////
		Another thing you would probably like to do is look at the trigger waveform that comes from channel 33 (TR0) and determine the time of it's leading edge. This is done using the
		function 

			DataPoint SignalAnalyzer::FindTriggerTime(Channels_t& a_vAllChannels)

		a_vAllChannels is the vector of vector of channels we are familiar with, including channel 33 (TR0) and the return value is a DataPoint instance representing the point at which
		the trigger waveform passes the threshold defined in the configuration as trigger-threshold-volts.
	

	*/

	
	
	// This method allows the figures generated by ROOT to be manipulated during analysis, otherwise they freeze and you have to wait until the end to do anything with them (save them, move them, etc.)
	ProcessEvents();
}

void SignalAnalyzerMicrocavity::Flush()
{
	if(m_iFlags & ETriggerTimingMonitor)
	{
		m_pTriggerTimingMonitor->Flush();
	}
}

