DICTDIR = root_dictionary
OBJDIR = obj
BINDIR = bin

DICTNAME = dict
DICTSRC = $(patsubst %, $(DICTDIR)/%.cxx, $(DICTNAME))
DICTOBJ = $(patsubst %, $(OBJDIR)/%.o, $(DICTNAME))

all:	$(BINDIR) $(OBJDIR) $(DICTOBJ)
	@make -f make_acquisition
	@make -f make_analysis
	@make -f make_step
	@make -f make_scope
	@make -f make_compress

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
	-make -f make_acquisition clean
	-make -f make_analysis clean
	-make -f make_step clean
	-make -f make_scope clean
	-make -f make_compress clean
	-rm $(DICTDIR)/dict.cxx
	-rm $(BINDIR)/dict_rdict.pcm



clobber:	
	-make -f ./make_acquisition clobber
	-make -f ./make_analysis clobber
	-make -f ./make_step clobber
	-make -f ./make_scope clobber
	-make -f make_compress clobber


