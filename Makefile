DICTDIR = root_dictionary
OBJDIR = obj
BINDIR = bin

DICTNAME = dict
DICTSRC = $(patsubst %, $(DICTDIR)/%.cxx, $(DICTNAME))
DICTOBJ = $(patsubst %, $(OBJDIR)/%.o, $(DICTNAME))


all:	$(DICTOBJ)
	@cd ./analysis; make -f make_lib
	@make -f ./make_digitizer_acquisition
	@cd ./analysis; make

$(DICTSRC): $(DICTDIR)/LinkDef.h
	@rootcint -f $@ -p $^
	@mv $(DICTDIR)/dict_rdict.pcm $(BINDIR)/

$(DICTOBJ): $(BINDIR) $(OBJDIR) $(DICTSRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

$(BINDIR):
	@mkdir $(BINDIR)

$(OBJDIR):
	@mkdir $(OBJDIR)

clean:
	@make -f ./make_digitizer_acquisition clean
	@cd ./analysis; make clean

clobber:	
	@make -f ./make_digitizer_acquisition clobber
	@cd ./analysis; make clobber

