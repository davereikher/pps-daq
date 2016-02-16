ROOTLIBS = $(shell root-config --libs) -lRooFitCore -lMinuit -lEG 
ROOTINCS = $(shell root-config --cflags) 
ROOTLIBS = -L$(ROOTSYS)/lib  -lHtml -lCore -lTreePlayer -lRIO -lNet -lssl -lcrypto -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lm -ldl -rdynamic -lGenVector -lTMVA
MYFLAG = -I./include
#CXX = g++
DEBUG =
ROOTFILEOUTDIR = ./root
CXXFLAGS = -D ROOT_FILE_OUT_DIR=\"$(ROOTFILEOUTDIR)\" $(MYFLAG) -fpermissive $(ROOTINCS) -Wall -Wno-return-type -Wunused -Wno-deprecated
#CXXFLAGS        =-I$(ROOTSYS)/include -O -Wall -fPIC
LD = $(CXX)
###LDLIBS = $(ROOTLIBS) -lz -lpthread
LDLIBS = $(ROOTLIBS) -lpthread -lCAENDigitizer
LDFLAGS = $(MYFLAG)

OBJDIR = obj
SOURCEDIR = src
BINDIR = bin
DICTDIR = root_dictionary
ROOTOUTDIR = root
LIBDIR = lib

EXECS = $(BINDIR)/v1742-readout
_OBJECTS = Plotter.o keyb.o ProprietaryUtils.o EventHandler.o Exception.o dict.o DigitizerManager.o main.o
OBJECTS = $(patsubst %,$(OBJDIR)/%,$(_OBJECTS))

#.SUFFIXES: .o .C .c .cpp .so .cxx

all : $(EXECS)

$(BINDIR)/v1742-readout: $(OBJDIR) $(BINDIR) $(DICTDIR) $(ROOTOUTDIR) $(OBJECTS) 
	@echo "=> linking $@"	
	@echo $(LDFLAGS) 
	$(LD) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

$(OBJDIR):
	@mkdir $@

$(BINDIR):
	@mkdir $@

$(ROOTOUTDIR):
	@mkdir $@


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

$(DICTDIR)/dict.cxx: $(DICTDIR)/LinkDef.h
	@rootcint -f $@ -p $^
	@mv $(DICTDIR)/dict_rdict.pcm $(BINDIR)/

$(OBJDIR)/dict.o: $(DICTDIR)/dict.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

.PHONY : clean clobber
clean :
	@echo "=> removing object files"
	@rm -f $(OBJDIR)/*.o 
	@rm $(DICTDIR)/dict.cxx

clobber : clean
	@echo "=> removing executables"
	@rm -f $(EXECS)
