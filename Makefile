########################################################################
#
##              --- CAEN SpA - Computing Division ---
#
##   CAENDigitizer Software Project
#
##   Created  :  October    2009      (Rel. 1.0)
#
##   Auth: A. Lucchesi
#
#########################################################################
ARCH	=	`uname -m`

OUTDIR  =    	./bin/$(ARCH)/Release/
OUTNAME =    	ReadoutTest_Digitizer.bin
OUT     =    	$(OUTDIR)/$(OUTNAME)

#CC	=	g++

#COPTS	=	-fPIC -DLINUX -O2

#FLAGS	=	-soname -s
#FLAGS	=       -Wall,-soname -s
#FLAGS	=	-Wall,-soname -nostartfiles -s
#FLAGS	=	-Wall,-soname

DEPLIBS	=	-lCAENDigitizer

LIBS	=	-L..

INCLUDEDIR =	-I./include

OBJS	=	src/ReadoutTest_Digitizer.o src/keyb.o src/ProprietaryUtils.o src/Event.o src/Exception.o

CPPFLAGS = -I./include

INCLUDES =	./include/*

EVENTS_DIR = /tmp/vme

#########################################################################

all	:	$(OUT)

clean	:
		/bin/rm -f $(OBJS) $(OUT)

clean-events	:
		/bin/rm -f $(OBJS) $(OUT)
		/bin/rm $(EVENTS_DIR)/*

$(OUT)	:	$(OBJS)
		/bin/rm -f $(OUT)
		if [ ! -d $(OUTDIR) ]; then mkdir -p $(OUTDIR); fi
		$(CXX) -o $(OUT) $(OBJS) $(DEPLIBS)
		

$(OBJS)	:	$(INCLUDES) Makefile

%.o	:	%.c
		$(CXX) $(INCLUDEDIR) -c -o $@ $<

