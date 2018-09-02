#########################################################################
# FILE NAME Makefile
#           copyright 2006 by University of Wales Aberystwyth
#
# BRIEF MODULE DESCRIPTION
#           Makefile for the SW_MAPS test programs
#
# History:
#
#  14.05.2008 - initial version by Martin Huelse (msh@aber.ac.uk)
#
#
#########################################################################

CC         = g++

BIN_PATH = ../../BIN

LIB_PATH_LOCAL = ../../LIB
LIB_PATH_GLOBAL = /usr/local/lib

INCL_LOCAL = -I../../INCLUDE -I../SRC 
INCL_GLOBAL = -I/usr/local/include

INCL = $(INCL_LOCAL) $(INCL_GLOBAL)
LIBS = -I$(LIB_PATH_LOCAL) -I$(LIB_PATH_GLOBAL) -lstdc++

KERNELPATH=/lib/modules/2.6.17-10-generic/build

CFLAGS     = -I$(KERNELPATH)/include 
CFLAGS    += -Wall  -fomit-frame-pointer
CFLAGS    += -O2
CFLAGS    += -DOSIF_OS_LINUX

all:	CBaseMapTest CBaseMapExtTest CXmlMapTest CXmlMapExtTest CMemoryMapTest

CBaseMapExtTest.o: ./CBaseMapExtTest.cpp ../SRC/CBaseMap.cpp ../SRC/CBaseMap.h 
	$(CC) -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<

CBaseMapExtTest: CBaseMapExtTest.o ./CBaseMapExtTest.cpp ../SRC/CBaseMap.cpp ../SRC/CBaseMap.h  
	$(CC) -o  CBaseMapExtTest CBaseMapExtTest.o \
	../SRC/CBaseMap.o \
	$(LIBS) 


CBaseMapTest.o: ./CBaseMapTest.cpp ../SRC/CBaseMap.cpp ../SRC/CBaseMap.h 
	$(CC) -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<

CBaseMapTest: CBaseMapTest.o ./CBaseMapTest.cpp ../SRC/CBaseMap.cpp ../SRC/CBaseMap.h  
	$(CC) -o  CBaseMapTest CBaseMapTest.o \
	../SRC/CBaseMap.o \
	$(LIBS) 


CXmlMapTest.o: ./CXmlMapTest.cpp ./CXmlMapTest.cpp ../SRC/CXmlMap.h ../SRC/CXmlMap.cpp ../SRC/CBaseMap.cpp ../SRC/CBaseMap.h 
	$(CC) -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<

CXmlMapTest: CXmlMapTest.o ./CXmlMapTest.cpp  ../SRC/CXmlMap.h ../SRC/CXmlMap.cpp ../SRC/CBaseMap.cpp ../SRC/CBaseMap.h  
	$(CC) -o  CXmlMapTest CXmlMapTest.o \
	../SRC/CXmlMap.o ../SRC/CBaseMap.o  -lxerces-c \
	$(LIBS) 


CXmlMapExtTest.o: ./CXmlMapExtTest.cpp ./CXmlMapExtTest.cpp ../SRC/CXmlMap.h ../SRC/CXmlMap.cpp ../SRC/CBaseMap.cpp ../SRC/CBaseMap.h 
	$(CC) -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<

CXmlMapExtTest: CXmlMapExtTest.o ./CXmlMapExtTest.cpp  ../SRC/CXmlMap.h ../SRC/CXmlMap.cpp ../SRC/CBaseMap.cpp ../SRC/CBaseMap.h  
	$(CC) -o  CXmlMapExtTest CXmlMapExtTest.o \
	../SRC/CXmlMap.o ../SRC/CBaseMap.o  -lxerces-c \
	$(LIBS) 



CMemoryMapTest.o: ./CMemoryMapTest.cpp ./CMemoryMapTest.cpp ../SRC/CMemoryMap.h ../SRC/CMemoryMap.cpp ../SRC/CBaseMap.cpp ../SRC/CBaseMap.h 
	$(CC) -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<

CMemoryMapTest: CMemoryMapTest.o ./CMemoryMapTest.cpp  ../SRC/CMemoryMap.h ../SRC/CMemoryMap.cpp ../SRC/CBaseMap.cpp ../SRC/CBaseMap.h  
	$(CC) -o  CMemoryMapTest CMemoryMapTest.o \
	../SRC/CMemoryMap.o ../SRC/CBaseMap.o ../SRC/CXmlMap.o   -lxerces-c \
	$(LIBS) 



#cleaning up
clean:
	rm *.o ./*Test cpyMap.xml map.xml *~ 

