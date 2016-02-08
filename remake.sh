#!/bin/bash
make clean-events
make
./bin/x86_64/Release/ReadoutTest_Digitizer.bin
