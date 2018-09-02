#########################################################################
# FILE NAME Makefile
#           copyright 2007 by University of Wales Aberystwyth
#
# BRIEF MODULE DESCRIPTION
#           Makefile for the small test programms 
#           for the use of maps.
#
# History:
#
#  14.05.2008 - initial version 
#
#
#########################################################################

CC         = g++

BIN_PATH = ../../BIN

LIB_PATH_LOCAL = ../../LIB
LIB_PATH_GLOBAL = /usr/local/lib
LIB_PATH_OPENCV = -L$(shell pkg-config --libs opencv)

INCL_LOCAL = -I../../INCLUDE -I../../LIB -I../SRC 
INCL_GLOBAL = -I/usr/local/include -I/usr/local/include/REVERB
INCL_OPENCV = -I$(shell kg-config --cflags opencv)



INCL = $(INCL_LOCAL) $(INCL_GLOBAL) $(INCL_OPENCV)
LIBS = -I$(LIB_PATH_LOCAL) -I$(LIB_PATH_GLOBAL)  $(LIB_PATH_OPENCV)


KERNELPATH=/lib/modules/2.6.17-10-generic/build

CFLAGS     = -I$(KERNELPATH)/include 
CFLAGS    += -Wall -Wstrict-prototypes -fomit-frame-pointer
CFLAGS    += -O2
CFLAGS    += -DOSIF_OS_LINUX


TARGETS= swapInputOutputSpace mappingPlug printMap simProprioCalibMaplearning simVisionCalibMaplearning mappingAdd mappingDotDiff mappingDiff xmlReader simpleXmlMapReader countNeighbourNodes memoryMap activationMap mapViewColoured mapView printComponent sim1dMapLearning simVisionMapLearning

all:	$(TARGETS)

xmlReader.o:	xmlReader.cpp
	$(CC) -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<
	
xmlReader: xmlReader.o ./xmlReader.cpp  
	$(CC) -o  xmlReader xmlReader.o \
	 -lxerces-c  \
	$(LIBS) 

simpleXmlMapReader.o:	simpleXmlMapReader.cpp
	$(CC) -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<
	
simpleXmlMapReader: simpleXmlMapReader.o ./simpleXmlMapReader.cpp  
	$(CC) -o  simpleXmlMapReader simpleXmlMapReader.o \
	 -lxerces-c  ../SRC/CXmlMap.o ../SRC/CBaseMap.o \
	$(LIBS) 

countNeighbourNodes.o:	countNeighbourNodes.cpp
	$(CC) -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<
	
countNeighbourNodes: countNeighbourNodes.o ./countNeighbourNodes.cpp  
	$(CC) -o  countNeighbourNodes countNeighbourNodes.o \
	 -lxerces-c  ../SRC/CBaseMap.o \
	$(LIBS) 


memoryMap.o:	memoryMap.cpp
	$(CC) -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<
	
memoryMap: memoryMap.o ./memoryMap.cpp  
	$(CC) -o  memoryMap memoryMap.o \
	 -lxerces-c  ../SRC/CXmlMap.o ../SRC/CBaseMap.o \
	$(LIBS) 


activationMap.o:	activationMap.cpp
	$(CC) $(INCL)  -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<
	
activationMap: activationMap.o ./activationMap.cpp  
	$(CC) -o  activationMap activationMap.o \
	 -lxerces-c  \
	 ../SRC/CXmlMap.o ../SRC/CBaseMap.o ../SRC/CMemoryMap.o \
	$(LIBS) 

sim1dMapLearning.o:	sim1dMapLearning.cpp
	$(CC) $(INCL)  -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<
	
sim1dMapLearning: sim1dMapLearning.o ./sim1dMapLearning.cpp  
	$(CC) -o  sim1dMapLearning sim1dMapLearning.o \
	 -lxerces-c  \
	 ../SRC/CXmlMap.o ../SRC/CBaseMap.o \
	$(LIBS) 


simVisionMapLearning.o:	simVisionMapLearning.cpp
	$(CC) $(INCL)  -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<
	
simVisionMapLearning: simVisionMapLearning.o ./simVisionMapLearning.cpp  
	$(CC) -o  simVisionMapLearning simVisionMapLearning.o \
	 -lxerces-c  \
	 ../SRC/CXmlMap.o ../SRC/CBaseMap.o \
	$(LIBS) 


simVisionCalibMaplearning.o:	simVisionCalibMaplearning.cpp
	$(CC) $(INCL)  -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<

simVisionCalibMaplearning:	simVisionCalibMaplearning.o ./simVisionCalibMaplearning.cpp  
	$(CC) -o  simVisionCalibMaplearning simVisionCalibMaplearning.o \
	 -lxerces-c  \
	 ../SRC/CXmlMap.o ../SRC/CBaseMap.o \
	$(LIBS) 



simProprioCalibMaplearning.o:	simProprioCalibMaplearning.cpp
	$(CC) $(INCL)  -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<

simProprioCalibMaplearning:	simProprioCalibMaplearning.o ./simProprioCalibMaplearning.cpp  
	$(CC) -o  simProprioCalibMaplearning simProprioCalibMaplearning.o \
	 -lxerces-c   \
	 ../SRC/CXmlMap.o ../SRC/CBaseMap.o \
	$(LIBS) 


mapView.o:	mapView.cpp
	$(CC) $(INCL)  -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<
	
mapView: mapView.o ./mapView.cpp  
	$(CC) -o  mapView mapView.o \
	 -lxerces-c  \
	 ../SRC/CXmlMap.o ../SRC/CBaseMap.o \
	$(LIBS) 

printMap.o:	printMap.cpp
	$(CC) $(INCL)  -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<
	
printMap: printMap.o ./printMap.cpp  
	$(CC) -o  printMap printMap.o \
	 -lxerces-c  \
	 ../SRC/CXmlMap.o ../SRC/CBaseMap.o \
	$(LIBS) 


mapViewColoured.o:	mapViewColoured.cpp
	$(CC) $(INCL)  -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<
	
mapViewColoured: mapViewColoured.o ./mapViewColoured.cpp  
	$(CC) -o  mapViewColoured mapViewColoured.o \
	 -lxerces-c   \
	 ../SRC/CXmlMap.o ../SRC/CBaseMap.o \
	$(LIBS) 


printComponent.o:	printComponent.cpp
	$(CC) $(INCL)  -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<
	
printComponent: printComponent.o ./printComponent.cpp  
	$(CC) -o  printComponent printComponent.o \
	 -lxerces-c  \
	 ../SRC/CXmlMap.o ../SRC/CBaseMap.o \
	$(LIBS) 


mappingDiff.o:	mappingDiff.cpp
	$(CC) $(INCL)  -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<
	
mappingDiff: mappingDiff.o ./mappingDiff.cpp  
	$(CC) -o  mappingDiff mappingDiff.o \
	 -lxerces-c   \
	 ../SRC/CXmlMap.o ../SRC/CBaseMap.o \
	$(LIBS) 


mappingDotDiff.o:	mappingDotDiff.cpp
	$(CC) $(INCL)  -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<
	
mappingDotDiff: mappingDotDiff.o ./mappingDotDiff.cpp  
	$(CC) -o  mappingDotDiff mappingDotDiff.o \
	 -lxerces-c  \
	 ../SRC/CXmlMap.o ../SRC/CBaseMap.o \
	$(LIBS) 


mappingAdd.o:	mappingAdd.cpp
	$(CC) $(INCL)  -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<
	
mappingAdd: mappingAdd.o ./mappingAdd.cpp  
	$(CC) -o  mappingAdd mappingAdd.o \
	 -lxerces-c   \
	 ../SRC/CXmlMap.o ../SRC/CBaseMap.o \
	$(LIBS) 



mappingPlug.o:	mappingPlug.cpp
	$(CC) $(INCL)  -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<
	
mappingPlug: mappingPlug.o ./mappingPlug.cpp  
	$(CC) -o  mappingPlug mappingPlug.o \
	 -lxerces-c  \
	 ../SRC/CXmlMap.o ../SRC/CBaseMap.o \
	$(LIBS) 

swapInputOutputSpace.o:	swapInputOutputSpace.cpp
	$(CC) $(INCL)  -I../SRC  -g -DLINUX -D__LINUX__ -DUNIX  -c $<
	
swapInputOutputSpace: swapInputOutputSpace.o ./swapInputOutputSpace.cpp  
	$(CC) -o  swapInputOutputSpace swapInputOutputSpace.o \
	 -lxerces-c  \
	 ../SRC/CXmlMap.o ../SRC/CBaseMap.o \
	$(LIBS) 


#cleaning up
clean:
	rm *.o *~ *.txt *.ppm $(TARGETS)

