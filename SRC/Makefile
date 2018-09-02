#########################################################################
# FILE NAME Makefile
#           copyright 2006 by University of Wales Aberystwyth
#
# BRIEF MODULE DESCRIPTION
#           Makefile for the SW_MAPS (for sensory-motor
#           mappings) implemenation
#
# History:
#
#  15.05.2008 - initial version 
#
#
#########################################################################

CC         = g++

BIN_PATH = ../../BIN

LIB_PATH_LOCAL = ../../LIB
LIB_PATH_GLOBAL = /usr/local/lib

INCL_LOCAL = ../../INCLUDE -I../../LIB -I../SRC 
INCL_GLOBAL = /usr/local/include

INCL = -I$(INCL_LOCAL) -I$(INCL_GLOBAL)
LIBS = -I$(LIB_PATH_LOCAL) -I$(LIB_PATH_GLOBAL)  -lstdc++



KERNELPATH=/lib/modules/2.6.17-10-generic/build

CFLAGS     = -I$(KERNELPATH)/include 
CFLAGS    += -Wall -Wstrict-prototypes -fomit-frame-pointer
CFLAGS    += -O2
CFLAGS    += -DOSIF_OS_LINUX

TARGETS = CBaseMap.o CXmlMap.o CMemoryMap.o

all:	$(TARGETS)


lib:	$(TAGETS)
	ar rcs libMAPS.a $(TARGETS)
	
dynlib: $(TARGETS)
	$(CC)  -shared -Wl,-soname,libMAPS.so.1 -o libMAPS.so.1.0.1 \
	$(TARGETS)
	
	

CBaseMap.o:	CBaseMap.cpp CBaseMap.h
	$(CC) $(INCL) -g -DLINUX -D__LINUX__ -DUNIX -fPIC -c  $< 
	

CXmlMap.o:	CXmlMap.cpp CXmlMap.h
	$(CC) $(INCL) -g -DLINUX -D__LINUX__ -DUNIX -fPIC -c  $< 


CMemoryMap.o:	CMemoryMap.cpp CMemoryMap.h CBaseMap.h CBaseMap.cpp
	$(CC) $(INCL) -g -DLINUX -D__LINUX__ -DUNIX -fPIC -c  $< 
	
doc:
	doxygen Doxyfile 

#cleaning up
clean:
	rm -r *.o *lib*.so.* *lib*.a *~ DOXYGENDOC


