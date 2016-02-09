ROOTLIBS = $(shell root-config --libs) -lRooFitCore -lMinuit -lEG 
ROOTINCS = $(shell root-config --cflags) 
ROOTLIBS = -L$(ROOTSYS)/lib  -lHtml -lCore -lTreePlayer -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lm -ldl -rdynamic -lGenVector -lTMVA
MYFLAG = $(shell ./rootarch.sh)
CXX = g++
DEBUG =
CXXFLAGS = $(MYFLAG) -fpermissive $(ROOTINCS) -Wall -Wno-return-type -Wunused -Wno-deprecated
#CXXFLAGS        =-I$(ROOTSYS)/include -O -Wall -fPIC
LD = $(CXX)
###LDLIBS = $(ROOTLIBS) -lz -lpthread
LDLIBS = $(ROOTLIBS) -lpthread -lCAENDigitizer
LDFLAGS = $(MYFLAG)

EXECS = v1742-readout
OBJECTS = src/ReadoutTest_Digitizer.o src/keyb.o src/ProprietaryUtils.o src/Event.o src/Exception.o

.SUFFIXES: .o .C .cpp .so .cxx

all : $(EXECS)

v1742-readout: $(OBJECTS)
	@echo "=> linking $@"	
	@echo $(LDFLAGS) 
	$(LD) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

%.o: %.C
	@echo "=> compiling $<"
	@echo  $(CXXFLAGS) 
	$(CXX) $(CXXFLAGS) $(DEBUG) -c $<

%.o: %.cpp
	@echo "=> compiling $<"
	@echo  $(CXXFLAGS) 
	$(CXX) $(CXXFLAGS) $(DEBUG) -c $<

%.o: %.cxx
	@echo "=> compiling $<"
	@echo  $(CXXFLAGS) 
	$(CXX) $(CXXFLAGS) $(DEBUG) -c $<


.PHONY : clean clobber
clean :
	@echo "=> removing object files"
	@rm -f *.o 

clobber : clean
	@echo "=> removing executables"
	@rm -f $(EXECS)
