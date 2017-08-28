# pps-daq
DAQ system over VME with the V1742 digitizer board.

This project was built as a data acquisition and analysis system for a stack of detectors (A very detailed description can
be found [here](https://cds.cern.ch/record/2240692/files/CERN-THESIS-2016-225.pdf). The description of this software and the
interfacing with the digitizer is described in chapter 7 - DAQ Using a Digitizer)).

This may be used as a reference for interfacing with CAEN digitizers via USB. DigitizerManager.cpp is the relevant source file for that.
ProperietaryUtils.cpp also has some methods that implement various features the API for which did not work for some reason, 
through direct access to registers on the digitizer.
