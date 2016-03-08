DICTDIR = root_dictionary
OBJDIR = obj
BINDIR = bin

DICTNAME = dict
DICTSRC = $(patsubst %, $(DICTDIR)/%.cxx, $(DICTNAME))
DICTOBJ = $(patsubst %, $(OBJDIR)/%.o, $(DICTNAME))

all:	$(BINDIR) $(OBJDIR) $(DICTOBJ)
	@make -f make_digitizer_acquisition
	@make -f make_analysis
	@make -f make_step_through_events

$(DICTSRC): $(DICTDIR)/LinkDef.h
	@rootcint -f $@ -p $^
	@mv $(DICTDIR)/dict_rdict.pcm $(BINDIR)/

$(DICTOBJ): $(DICTSRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I$(shell root-config --cflags)

$(BINDIR):
	@mkdir $(BINDIR)

$(OBJDIR):
	@mkdir $(OBJDIR)

clean:
	-make -f make_digitizer_acquisition clean
	-make -f make_analysis clean
	-make -f make_step_through_events clean
	-rm $(DICTDIR)/dict.cxx
	-rm $(BINDIR)/dict_rdict.pcm



clobber:	
	-make -f ./make_digitizer_acquisition clobber
	-make -f ./make_analysis clobber
	-make -f ./make_step_through_events clobber


