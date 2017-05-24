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
m_pTriggerTimingMonitor(new TriggerTimingMonitor(milliseconds(Configuration::Instance().GetTriggerRateAveragingDurationSecs()))),
// m_pchMonitor(new TriggerTimingMonitor(milliseconds(Configuration::Instance().GetTriggerRateAveragingDurationSecs()))),
m_pPanelMonitor(new PanelMonitor("E")),
m_pChannelCountRateMonitor(new ChannelCountRateMonitor(milliseconds(Configuration::Instance().GetTriggerRateAveragingDurationSecs()))),
m_pChannelMonitor(new ChannelMonitor(milliseconds(Configuration::Instance().GetTriggerRateAveragingDurationSecs())))

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

	//	m_pChannelMonitor->GotTrigger(a_timeStamp);

	}
//**********************************************************************************************************************************************************
//SignalAnalyzerMicrocavity::SignalAnalyzerMicrocavity():
//m_pTriggerTimingMonitor_1(new TriggerTimingMonitor(milliseconds(Configuration::Instance().GetTriggerRateAveragingDurationSecs()))),
// m_pchMonitor(new TriggerTimingMonitor(milliseconds(Configuration::Instance().GetTriggerRateAveragingDurationSecs()))),
//m_pPanelMonitor(new PanelMonitor("E")),
//m_pChannelMonitor(new ChannelMonitor(milliseconds(Configuration::Instance().GetTriggerRateAveragingDurationSecs())))
//{}
//**********************************************************************************************************************************************************

// m_pchMonitor->GotTrigger(a_timeStamp);
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
//	CountNumberOfPulses(a_timeStamp, vNormalizedChannels);

	if (m_iFlags & AnalysisFlagsMicrocavity::EPanelMonitor)
	{
		FindOriginalPulseInChannelRange(vNormalizedChannels, "E", Configuration::Instance().GetRanges()["E"]);
		m_pPanelMonitor->GotEvent(a_timeStamp, m_markers.m_vChannelsWithPulse);
	}


	for (int i = 0; i < 32; i++)
	{

		std::vector<int> range;
		range.push_back(i);
		FindOriginalPulseInChannelRange(vNormalizedChannels, "E", range);
		int number_of_detected_pulses = m_markers.m_vChannelsWithPulse.size();
		if (number_of_detected_pulses > 0)
		{		
			if(i>7 && i<16)
			{
			m_pChannelMonitor->GotTrigger(a_timeStamp, i-7);
			m_pChannelCountRateMonitor->GotTrigger(a_timeStamp, i-7);
			}
		}
		
	}




//	AfterPulse(a_timeStamp, vNormalizedChannels);
//***************************************************************************
//	AfterPulseMeasurement(a_timeStamp, vNormalizedChannels);
//	AfterPulseMeasurement_10(a_timeStamp, vNormalizedChannels);
//	AfterPulseMeasurement_20(a_timeStamp, vNormalizedChannels);
//	CountRates(a_timeStamp, vNormalizedChannels);
//****************************************************************************
	fflush(stdout);
	// This method allows the figures generated by ROOT to be manipulated during analysis, otherwise they freeze and you have to wait until the end to do anything with them (save them, move them, etc.)
	ProcessEvents();
}

void SignalAnalyzerMicrocavity::CountNumberOfPulses(nanoseconds a_timeStamp, Channels_t& a_vChannels)
{
	printf("------------------------\n");
	for (int i = 0; i < 32; i++)
	{
		//Go over the first 32 channels. We'll treat channel 33 (TR0) later.
//		std::tuple<DataPoint, DataPoint> dp = FindLeadingEdgeAndPulseExtremum(vNormalizedChannels[i]);
		std::vector<int> range;
		range.push_back(i);
		FindOriginalPulseInChannelRange(a_vChannels, "E", range);
		int number_of_detected_pulses = m_markers.m_vChannelsWithPulse.size();
		if (number_of_detected_pulses > 0)
		{
			printf("channel %d, number pulses detected:%d\n", i,number_of_detected_pulses);
		}		
	}
}

void SignalAnalyzerMicrocavity::CountRates(nanoseconds a_timeStamp, Channels_t& a_vChannels)
{
	static long counts[32]= {};
        static long data_count[8]= {};
	static int file_saving_flag=0;

	int rate_timer_seconds = Configuration::Instance().GetRateTimer();
	static nanoseconds Ini_rate_timer = duration_cast<nanoseconds>(seconds(rate_timer_seconds)); // modified by achintya
	static nanoseconds rate_timer= Ini_rate_timer; // modified by achintya
	static nanoseconds ini_time; // modified by achintya
	int Applied_Voltage = Configuration::Instance().GetApplied_Voltage();

	FILE *fp1;
//        FILE *Afterpulse_file = fopen("Afterpulse_file", "a");


	for (int i = 0; i < 32; i++)
	{
		//Go over the first 32 channels. We'll treat channel 33 (TR0) later.
//		std::tuple<DataPoint, DataPoint> dp = FindLeadingEdgeAndPulseExtremum(vNormalizedChannels[i]);
		std::vector<int> range;
		range.push_back(i);
		FindOriginalPulseInChannelRange(a_vChannels, "E", range);
		int number_of_detected_pulses = m_markers.m_vChannelsWithPulse.size();
		if (number_of_detected_pulses > 0)
		{
			counts[i] ++;
		//	printf("Pulse detected at channel %d at time %ld ns from start of run\n", i, a_timeStamp);

			if(i==15)
			{
				if(counts[i]==1)
				{	
				ini_time=a_timeStamp;
				}
//			printf("%d-th pulse detected at channel %d at time %ld ns\t\t or %ld ms\t\t or %ld sec\t\t from start of run\n",counts[i], i, a_timeStamp-ini_time,duration_cast<milliseconds>(a_timeStamp-ini_time).count(), duration_cast<seconds>(a_timeStamp-ini_time).count());
			}
		}		
	}


//*******************************************************************************************************************************************************************************************************
	printf("rate_timer is %ld ns, time stamp is %ld seconds or %ld milliseconds\n", rate_timer, duration_cast<seconds>(a_timeStamp).count(), duration_cast<milliseconds>(a_timeStamp).count());//   *
//*******************************************************************************************************************************************************************************************************
	if (a_timeStamp > rate_timer)
	{
		rate_timer += a_timeStamp;
//		rate_timer = a_timeStamp + Ini_rate_timer; // modified by achintya
		

		printf("Time's up. Summary of rates:\n\n");
		for (int i = 0; i < 32; i++)
		{
			if(i>7 && i<16)
			{
			printf("\tChannel %d\t\t\tNumber of pulses:%d\t\t\tRate:%f\n", i-7, counts[i], float(counts[i])/duration_cast<seconds>(a_timeStamp).count());// added by achintya i-7 is added instead of i to be consistent with our channel convention.
			
			data_count[i-8]=counts[i];			
			}				
		}
		printf("Summary rate in one line\n"); 
		if(file_saving_flag==0)
		{
			fp1 = fopen("./microcavity_out_afterpulse/data_count.txt", "a");
			fprintf(fp1, "%d", Applied_Voltage);
			fprintf(fp1, "\t");
			for(int i=0;i<8;i++)
			{
			printf("%d", data_count[i]) ;
			printf("\t");
			fprintf(fp1, "%d", data_count[i]);
			fprintf(fp1, "\t");
			}
			printf("\n");
			fprintf(fp1, "\n");

			file_saving_flag++;
		}
	int fclose( FILE * fp1);


	}


}

// added by achintya to count after pulse*****************************************************************************************************
void SignalAnalyzerMicrocavity::AfterPulse(nanoseconds a_timeStamp, Channels_t& a_vChannels)
{
	static long counts[32]= {};
	static long each_trail_counts[32]= {};
	static int Number_of_Afterpulse [32][15]={};

	

	int AfterPulse_timer_milliseconds = Configuration::Instance().GetAfterPulse_timer();
	static nanoseconds Ini_AfterPulse_timer = duration_cast<nanoseconds>(milliseconds(AfterPulse_timer_milliseconds));
	static nanoseconds AfterPulse_timer= Ini_AfterPulse_timer;
	static nanoseconds line_trigger[32]={};
	//static nanoseconds test_timer;	
	//printf("afterpulse millisec is %ld, afterpulse time in nanosec%ld and timestamp is %ld", AfterPulse_timer_milliseconds, AfterPulse_timer, a_timeStamp);



//************************************************************************************************************************************************************************************************************
	if (a_timeStamp > AfterPulse_timer)
	{
		AfterPulse_timer = a_timeStamp + Ini_AfterPulse_timer;
//		
		printf("afterTime's up. Summary of rates: time stump %ld ns and %ld milliseconds\n",a_timeStamp,duration_cast<milliseconds>(a_timeStamp).count());
//		printf("afterTime's up. Summary of rates: AfterPulse_timer %ld ns and %ld milliseconds\n",AfterPulse_timer,duration_cast<milliseconds>(AfterPulse_timer).count());
//
//		printf("afterTime's up. Summary of rates: Ini_AfterPulse_timer %ld ns and %ld milliseconds\n",Ini_AfterPulse_timer,duration_cast<milliseconds>(Ini_AfterPulse_timer).count());
//******************************************************************************************************************************************************************************************************
		for (int i = 0; i < 32; i++)
		{
			

			printf("\tCount[%d]=%d\t\t each_trail_counts[%d]=%d\n",i,counts[i],i,each_trail_counts[i]);
				
			if (Number_of_Afterpulse [i][0]<100 && each_trail_counts[i]>0)// && a_timeStamp<line_trigger[i])
			{


				if(each_trail_counts[i]>10)
				{
				each_trail_counts[i]=12;
				}
				Number_of_Afterpulse [i][each_trail_counts[i]] ++;   
				Number_of_Afterpulse [i][0]=Number_of_Afterpulse [i][0]+1;
			}


			//printf("\tCount[i]=%d\t\t each_trail_counts[i]=%d\n", counts[i],each_trail_counts[i]);
			//printf("line_trigger[%d]=%ld\n",i,line_trigger[i]);
			//if(a_timeStamp > line_trigger[i])
			//{

			//	printf("line_trigger[%d]=%ld,a_timeStamp=%ld\n",i,duration_cast<milliseconds>(line_trigger[i]).count(),duration_cast<milliseconds>(a_timeStamp).count());
				each_trail_counts[i]=0;	
			//}	

		}


		    for(int x=8;x<16;x++)
		{
		//if(x==14)	
		//    {
			for(int y=0;y<15;y++)
			{
			printf("%d", Number_of_Afterpulse[x][y]) ;
   			printf("\t");
			}
		    printf("\n");
		//    }
		}	

//************************************************************************************************************************************************************************************************************		
	}

//************************************************************************************************************************************************************************************************************
	for (int i = 0; i < 32; i++)
	{
		//Go over the first 32 channels. We'll treat channel 33 (TR0) later.
//		std::tuple<DataPoint, DataPoint> dp = FindLeadingEdgeAndPulseExtremum(vNormalizedChannels[i]);
		std::vector<int> range;
		range.push_back(i);
		FindOriginalPulseInChannelRange(a_vChannels, "E", range);
		int number_of_detected_pulses = m_markers.m_vChannelsWithPulse.size();
		if (number_of_detected_pulses > 0)
		{
		
			if(each_trail_counts[i]==0)
				{
				line_trigger[i]=a_timeStamp + Ini_AfterPulse_timer;
				}
			
			counts[i] ++;
			each_trail_counts[i] ++;
			printf("lower loop afterPulse detected at channel %d at time %ld ns from after start of run\n", i, a_timeStamp);

	//		printf("lower loop count[i]= %d each_trail_counts[i]= %d\n", counts[i], each_trail_counts[i]);
	//		printf("number_of_detected_pulses= %d", number_of_detected_pulses);
		}
		   
		
		
	}





/*/++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		for (int i = 0; i < 32; i++)
		{
			//Go over the first 32 channels. We'll treat channel 33 (TR0) later.
			std::tuple<DataPoint, DataPoint> dp = FindLeadingEdgeAndPulseExtremum(a_vChannels[i]);
			
			//Do something with the points, like pass them to a monitor.
			//Use the following syntax to access elements of tuples:
			//To get the time value in ns of the first element (leading edge):
			std::get<EDGE_THRES_INDEX>(dp).GetX();
			//Double test_timer=std::get<EDGE_THRES_INDEX>(dp).GetX();

			printf("pulse edge %ld ns from after start of run\n", std::get<EDGE_THRES_INDEX>(dp).GetX());
			
			//To get the second element (pulse extremum), it's the same but instead of EDGE_THRES_INDEX use MIN_PULSE_INDEX. These constants are defined in Types.h.
		}*/



}// end of function afterpulse
//********************************************************************************
void SignalAnalyzerMicrocavity::AfterPulseMeasurement(nanoseconds a_timeStamp, Channels_t& a_vChannels)
{
	static long counts[32]= {};
	static long each_trail_counts[32]= {};
	static int Number_of_Afterpulse [32][15]={};
	static int file_saving_flag=0;
	int rate_timer_seconds = Configuration::Instance().GetRateTimer();
	static nanoseconds rate_timer = duration_cast<nanoseconds>(seconds(rate_timer_seconds));
	int Applied_Voltage = Configuration::Instance().GetApplied_Voltage();
	FILE *fp1;
	FILE *fp2;
	int AfterPulse_timer_milliseconds = Configuration::Instance().GetAfterPulse_timer();
	static nanoseconds Ini_AfterPulse_timer = duration_cast<nanoseconds>(milliseconds(AfterPulse_timer_milliseconds));
	static nanoseconds AfterPulse_timer= Ini_AfterPulse_timer;
	static nanoseconds line_trigger[32]={};
	static nanoseconds prev_line_trigger[32]={};

//m_pTriggerTimingMonitor(new TriggerTimingMonitor(milliseconds(Configuration::Instance().GetTriggerRateAveragingDurationSecs()))),

//		m_pTriggerTimingMonitor->GotTrigger(a_timeStamp);




	for (int i = 0; i < 32; i++)
	{

		std::vector<int> range;
		range.push_back(i);
		FindOriginalPulseInChannelRange(a_vChannels, "E", range);
		int number_of_detected_pulses = m_markers.m_vChannelsWithPulse.size();
		if (number_of_detected_pulses > 0)
		{
			line_trigger[i]=a_timeStamp;
			counts[i] ++;
			each_trail_counts[i] ++;
		}
		
	}


	for (int i = 0; i < 32; i++)
	{

		if (line_trigger[i]- prev_line_trigger[i] > Ini_AfterPulse_timer)
		{
			
				
			if (Number_of_Afterpulse [i][0]<1000 && each_trail_counts[i]>0)
			{
				if(each_trail_counts[i]>10)
				{
				each_trail_counts[i]=12;
				}
				Number_of_Afterpulse [i][each_trail_counts[i]] ++;   
				Number_of_Afterpulse [i][0]=Number_of_Afterpulse [i][0]+1;
			}

			each_trail_counts[i]=0;
			prev_line_trigger[i]=line_trigger[i];	
			
		}
	}
printf("file_saving_flag %d\n",file_saving_flag);

	if (a_timeStamp > rate_timer && file_saving_flag==0)
	{	
		fp1 = fopen("./microcavity_out_afterpulse/after_pulse_count1.txt", "a");
		fp2 = fopen("./microcavity_out_afterpulse/after_pulse_data1.txt", "a");

		fprintf(fp1, "%d", Applied_Voltage);
		fprintf(fp1, "\t");

		fprintf(fp2, "Applied_Voltage %dV\t Afterpulse time window %ldms\t Count time %ldsec\n", Applied_Voltage, duration_cast<milliseconds>(Ini_AfterPulse_timer).count(), duration_cast<seconds>(rate_timer).count());

		file_saving_flag++;


		printf("Summary of afterpulse adjusted counts after %ld second in one line with afterpulse timer %ld milliseconds\n",duration_cast<seconds>(rate_timer).count(), duration_cast<milliseconds>(Ini_AfterPulse_timer).count()); 
		for(int x=8;x<16;x++)
			{
			printf("%d", Number_of_Afterpulse[x][0]) ;
	   		printf("\t");
			fprintf(fp1, "%d", Number_of_Afterpulse[x][0]);
	   		fprintf(fp1, "\t");
			}
		printf("\n");
		fprintf(fp1, "\n");
		int fclose( FILE * fp1);


		for(int x=8;x<16;x++)
		{
			for(int y=0;y<15;y++)
			{
				fprintf(fp2, "%d", Number_of_Afterpulse[x][y]) ;
		   		fprintf(fp2, "\t");
			}
			fprintf(fp2, "\n");
		}	
		fprintf(fp2, "\n");





	}




	for(int x=8;x<16;x++)
	{
		for(int y=0;y<15;y++)
		{
			printf("%d", Number_of_Afterpulse[x][y]) ;
	   		printf("\t");
		}
		printf("\n");
	}	
	printf("\n");
		
			



}

void SignalAnalyzerMicrocavity::AfterPulseMeasurement_10(nanoseconds a_timeStamp, Channels_t& a_vChannels)
{
	static long counts[32]= {};
	static long each_trail_counts[32]= {};
	static int Number_of_Afterpulse [32][15]={};
	static int file_saving_flag=0;
	int rate_timer_seconds = Configuration::Instance().GetRateTimer();
	static nanoseconds rate_timer = duration_cast<nanoseconds>(seconds(rate_timer_seconds));
	int Applied_Voltage = Configuration::Instance().GetApplied_Voltage();
	FILE *fp1;
	FILE *fp2;
	int AfterPulse_timer_milliseconds = Configuration::Instance().GetAfterPulse_timer();
	static nanoseconds Ini_AfterPulse_timer = duration_cast<nanoseconds>(milliseconds(AfterPulse_timer_milliseconds+30));
	static nanoseconds AfterPulse_timer= Ini_AfterPulse_timer;
	static nanoseconds line_trigger[32]={};
	static nanoseconds prev_line_trigger[32]={};

		m_pTriggerTimingMonitor->GotTrigger(a_timeStamp); //tast


	for (int i = 0; i < 32; i++)
	{

		std::vector<int> range;
		range.push_back(i);
		FindOriginalPulseInChannelRange(a_vChannels, "E", range);
		int number_of_detected_pulses = m_markers.m_vChannelsWithPulse.size();
		if (number_of_detected_pulses > 0)
		{
			line_trigger[i]=a_timeStamp;
			counts[i] ++;
			each_trail_counts[i] ++;

		}
		
	}


	for (int i = 0; i < 32; i++)
	{

		if (line_trigger[i]- prev_line_trigger[i] > Ini_AfterPulse_timer)
		{
			
				
			if (Number_of_Afterpulse [i][0]<1000 && each_trail_counts[i]>0)
			{
				if(each_trail_counts[i]>10)
				{
				each_trail_counts[i]=12;
				}
				Number_of_Afterpulse [i][each_trail_counts[i]] ++;   
				Number_of_Afterpulse [i][0]=Number_of_Afterpulse [i][0]+1;
			}

			each_trail_counts[i]=0;
			prev_line_trigger[i]=line_trigger[i];	
			
		}
	}

printf("file_saving_flag %d\n",file_saving_flag);

	if (a_timeStamp > rate_timer && file_saving_flag==0)
	{	
		fp1 = fopen("./microcavity_out_afterpulse/after_pulse_count2.txt", "a");
		fp2 = fopen("./microcavity_out_afterpulse/after_pulse_data2.txt", "a");

		fprintf(fp1, "%d", Applied_Voltage);
		fprintf(fp1, "\t");

		fprintf(fp2, "Applied_Voltage %dV\t Afterpulse time window %ldms\t Count time %ldsec\n", Applied_Voltage, duration_cast<milliseconds>(Ini_AfterPulse_timer).count(), duration_cast<seconds>(rate_timer).count());

		file_saving_flag++;


		printf("Summary of afterpulse adjusted counts after %ld second in one line with afterpulse timer %ld milliseconds\n",duration_cast<seconds>(rate_timer).count(), duration_cast<milliseconds>(Ini_AfterPulse_timer).count()); 
		for(int x=8;x<16;x++)
			{
			printf("%d", Number_of_Afterpulse[x][0]) ;
	   		printf("\t");
			fprintf(fp1, "%d", Number_of_Afterpulse[x][0]);
	   		fprintf(fp1, "\t");
			}
		printf("\n");
		fprintf(fp1, "\n");
		int fclose( FILE * fp1);


		for(int x=8;x<16;x++)
		{
			for(int y=0;y<15;y++)
			{
				fprintf(fp2, "%d", Number_of_Afterpulse[x][y]) ;
		   		fprintf(fp2, "\t");
			}
			fprintf(fp2, "\n");
		}	
		fprintf(fp2, "\n");

	}


printf("After pusle with %ld\n",duration_cast<milliseconds>(Ini_AfterPulse_timer).count());

	for(int x=8;x<16;x++)
	{
		for(int y=0;y<15;y++)
		{
			printf("%d", Number_of_Afterpulse[x][y]) ;
	   		printf("\t");
		}
		printf("\n");
	}	
	printf("\n");
}


void SignalAnalyzerMicrocavity::AfterPulseMeasurement_20(nanoseconds a_timeStamp, Channels_t& a_vChannels)
{
	static long counts[32]= {};
	static long each_trail_counts[32]= {};
	static int Number_of_Afterpulse [32][15]={};
	static int file_saving_flag=0;
	int rate_timer_seconds = Configuration::Instance().GetRateTimer();
	static nanoseconds rate_timer = duration_cast<nanoseconds>(seconds(rate_timer_seconds));
	int Applied_Voltage = Configuration::Instance().GetApplied_Voltage();
	FILE *fp1;
	FILE *fp2;
	int AfterPulse_timer_milliseconds = Configuration::Instance().GetAfterPulse_timer();
	static nanoseconds Ini_AfterPulse_timer = duration_cast<nanoseconds>(milliseconds(AfterPulse_timer_milliseconds+60));
	static nanoseconds AfterPulse_timer= Ini_AfterPulse_timer;
	static nanoseconds line_trigger[32]={};
	static nanoseconds prev_line_trigger[32]={};

	for (int i = 0; i < 32; i++)
	{

		std::vector<int> range;
		range.push_back(i);
		FindOriginalPulseInChannelRange(a_vChannels, "E", range);
		int number_of_detected_pulses = m_markers.m_vChannelsWithPulse.size();
		if (number_of_detected_pulses > 0)
		{
			line_trigger[i]=a_timeStamp;
			counts[i] ++;
			each_trail_counts[i] ++;
		}
		
	}


	for (int i = 0; i < 32; i++)
	{

		if (line_trigger[i]- prev_line_trigger[i] > Ini_AfterPulse_timer)
		{
			
				
			if (Number_of_Afterpulse [i][0]<1000 && each_trail_counts[i]>0)
			{
				if(each_trail_counts[i]>10)
				{
				each_trail_counts[i]=12;
				}
				Number_of_Afterpulse [i][each_trail_counts[i]] ++;   
				Number_of_Afterpulse [i][0]=Number_of_Afterpulse [i][0]+1;
			}

			each_trail_counts[i]=0;
			prev_line_trigger[i]=line_trigger[i];	
			
		}
	}

printf("file_saving_flag %d\n",file_saving_flag);

	if (a_timeStamp > rate_timer && file_saving_flag==0)
	{	
		fp1 = fopen("./microcavity_out_afterpulse/after_pulse_count3.txt", "a");
		fp2 = fopen("./microcavity_out_afterpulse/after_pulse_data3.txt", "a");

		fprintf(fp1, "%d", Applied_Voltage);
		fprintf(fp1, "\t");

		fprintf(fp2, "Applied_Voltage %dV\t Afterpulse time window %ldms\t Count time %ldsec\n", Applied_Voltage, duration_cast<milliseconds>(Ini_AfterPulse_timer).count(), duration_cast<seconds>(rate_timer).count());

		file_saving_flag++;


		printf("Summary of afterpulse adjusted counts after %ld second in one line with afterpulse timer %ld milliseconds\n",duration_cast<seconds>(rate_timer).count(), duration_cast<milliseconds>(Ini_AfterPulse_timer).count()); 
		for(int x=8;x<16;x++)
			{
			printf("%d", Number_of_Afterpulse[x][0]) ;
	   		printf("\t");
			fprintf(fp1, "%d", Number_of_Afterpulse[x][0]);
	   		fprintf(fp1, "\t");
			}
		printf("\n");
		fprintf(fp1, "\n");
		int fclose( FILE * fp1);


		for(int x=8;x<16;x++)
		{
			for(int y=0;y<15;y++)
			{
				fprintf(fp2, "%d", Number_of_Afterpulse[x][y]) ;
		   		fprintf(fp2, "\t");
			}
			fprintf(fp2, "\n");
		}	
		fprintf(fp2, "\n");





	}


printf("After pusle with %ld\n",duration_cast<milliseconds>(Ini_AfterPulse_timer).count());

	for(int x=8;x<16;x++)
	{
		for(int y=0;y<15;y++)
		{
			printf("%d", Number_of_Afterpulse[x][y]) ;
	   		printf("\t");
		}
		printf("\n");
	}	
	printf("\n");
}

// addition ended by achintya********************************************************************************************************
void SignalAnalyzerMicrocavity::Flush()
{
	if(m_iFlags & ETriggerTimingMonitor)
	{
		m_pTriggerTimingMonitor->Flush();
	}
}
