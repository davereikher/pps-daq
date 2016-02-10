ROOTLIBS = $(shell root-config --libs) -lRooFitCore -lMinuit -lEG 
ROOTINCS = $(shell root-config --cflags) 
ROOTLIBS = -L$(ROOTSYS)/lib  -lHtml -lCore -lTreePlayer -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lm -ldl -rdynamic -lGenVector -lTMVA
MYFLAG = -I./include $(shell ./rootarch.sh)
#CXX = g++
DEBUG =
CXXFLAGS = $(MYFLAG) -fpermissive $(ROOTINCS) -Wall -Wno-return-type -Wunused -Wno-deprecated
#CXXFLAGS        =-I$(ROOTSYS)/include -O -Wall -fPIC
LD = $(CXX)
###LDLIBS = $(ROOTLIBS) -lz -lpthread
LDLIBS = $(ROOTLIBS) -lpthread -lCAENDigitizer
LDFLAGS = $(MYFLAG)

OBJDIR = obj
SOURCEDIR = src
BINDIR = bin

EXECS = $(BINDIR)/v1742-readout
#TODO: fix the object output path issue. Plotter appears in a different path than the others!
_OBJECTS = Plotter.o ReadoutTest_Digitizer.o keyb.o ProprietaryUtils.o Event.o Exception.o 
OBJECTS = $(patsubst %,$(OBJDIR)/%,$(_OBJECTS))

#.SUFFIXES: .o .C .c .cpp .so .cxx

all : $(EXECS)

$(BINDIR)/v1742-readout: $(OBJECTS)
	@echo "=> linking $@"	
	@echo $(LDFLAGS) 
	$(LD) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

$(OBJDIR)/%.o: $(SOURCEDIR)/%.C
	@echo "=> compiling $<"
	@echo  $(CXXFLAGS) 
	$(CXX) $(CXXFLAGS) $(DEBUG) -c $< -o $@

$(OBJDIR)/%.o: $(SOURCEDIR)/%.cpp
	@echo "=> compiling $<"
	@echo  $(CXXFLAGS) 
	$(CXX) $(CXXFLAGS) $(DEBUG) -c $< -o $@

$(OBJDIR)/%.o: $(SOURCEDIR)/%.cxx
	@echo "=> compiling $<"
	@echo  $(CXXFLAGS) 
	$(CXX) $(CXXFLAGS) $(DEBUG) -c $< -o $@

$(OBJDIR)/%.o: $(SOURCEDIR)/%.c
	@echo "=> compiling $<"
	@echo  $(CXXFLAGS) 
	$(CXX) $(CXXFLAGS) $(DEBUG) -c $< -o $@

.PHONY : clean clobber
clean :
	@echo "=> removing object files"
	@rm -f $(OBJDIR)/*.o 

clobber : clean
	@echo "=> removing executables"
	@rm -f $(EXECS)
