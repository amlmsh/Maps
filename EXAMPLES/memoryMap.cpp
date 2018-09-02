#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include <iostream>
#include <stdexcept>
#include <math.h>

#include "CXmlMap.h"

using namespace std;

XERCES_CPP_NAMESPACE_USE

void createRandomConnections(CXmlMap *map, int range, int nmbConnections);

int main (int argc, char* args[]) {
	CXmlMap map;
	
	int inpDim = 2;
	int outDim = 2;
	int range = 100;
	float connProb = 0.00005;
	
	map.init(2,2,1,5000);
	
	// create random connections / links
	int nmbConnections;
	nmbConnections = ((float)(range*range*range*range))*connProb;
	cout << "field: " << range << "x"<< range <<
		"   nmbConnections: " << nmbConnections << endl;
	createRandomConnections(&map, 100, nmbConnections);
	
	// write memory map 
	//map.writeXmlFile("memmap.xml");

	// random walk in the output space
	float* inpNode = new float[inpDim];
	float* outNode = new float[outDim];
	srand ( time(NULL) );
	
	int outputActivations = 0;
	int *res;
	int steps = 1000;
	for(int w = 0; w < steps; w++){
		for(int k = 0; k < inpDim; k++){
			inpNode[k] = (float)((int)(range*((float)rand()) / ((float)RAND_MAX)));
		};	


		res = map.getInputNodeIndicesWithinRegion(inpNode,0.0);
		if(res != NULL){
			outputActivations += res[0];
			delete res;
		};
	};
	cout << "avg. nmb of input nodes: " << 
		((float)outputActivations) / ((float)steps)  << endl; 
};



void createRandomConnections(CXmlMap *map, int range, int nmbConnections){
	map->clean();
	int inpDim = map->getInpDim();
	int outDim = map->getOutDim();
	if((inpDim < 1) || (outDim < 1)){
		return;
	};
	
	float* inpNode = new float[inpDim];
	float* outNode = new float[outDim];   
	srand ( time(NULL) );
	for(int i = 0; i < nmbConnections; i++){
		for(int k = 0; k < inpDim; k++){
			inpNode[k] = (float)((int)(range*((float)rand()) / ((float)RAND_MAX)));
	 	 };		
	 	 for(int l = 0; l < outDim; l++){
	 		 outNode[l] = (float)((int)(range*((float)rand()) / ((float)RAND_MAX)));
	 	 };
	 	 map->addLink(inpNode,outNode);
	 };
};
