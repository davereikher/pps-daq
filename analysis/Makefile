ROOTLIBS = $(shell root-config --libs) -lRooFitCore -lMinuit -lEG 
ROOTINCS = $(shell root-config --cflags) 
ROOTLIBS = -L$(ROOTSYS)/lib  -lHtml -lCore -lTreePlayer -lRIO -lNet -lssl -lcrypto -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lm -ldl -rdynamic -lGenVector -lTMVA
MYFLAG = -I./include -I../additional-packages/include -L../additional-packages/lib -fmax-errors=5
#CXX = g++
DEBUG =
CXXFLAGS = -D ROOT_FILE_OUT_DIR=\"$(ROOTFILEOUTDIR)\" $(MYFLAG) -fpermissive $(ROOTINCS) -Wall -Wno-return-type -Wunused -Wno-deprecated
#CXXFLAGS        =-I$(ROOTSYS)/include -O -Wall -fPIC
LD = $(CXX)
###LDLIBS = $(ROOTLIBS) -lz -lpthread
LDLIBS = $(ROOTLIBS) -lpthread -ljson
LDFLAGS = $(MYFLAG)

OBJDIR = obj
SOURCEDIR = src
BINDIR = bin
LIBDIR = lib

EXECS = $(BINDIR)/analysis
_OBJECTS = analysis.o SignalAnalyzer.o Configuration.o
OBJECTS = $(patsubst %,$(OBJDIR)/%,$(_OBJECTS))

#.SUFFIXES: .o .C .c .cpp .so .cxx

all : $(EXECS)

$(BINDIR)/analysis: $(OBJDIR) $(BINDIR) $(DICTDIR) $(ROOTOUTDIR) $(OBJECTS) 
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

#$(DICTDIR)/dict.cxx: $(DICTDIR)/LinkDef.h
#	@rootcint -f $@ -p $^
#	@mv $(DICTDIR)/dict_rdict.pcm $(BINDIR)/

#$(OBJDIR)/dict.o: $(DICTDIR)/dict.cxx
#	$(CXX) $(CXXFLAGS) -c $< -o $@ 

.PHONY : clean clobber
clean :
	@echo "=> removing object files"
	@rm -f $(OBJDIR)/*.o 

clobber : clean
	@echo "=> removing executables"
	@rm -f $(EXECS)
