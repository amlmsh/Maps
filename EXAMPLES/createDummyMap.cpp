/*
 * createDummyMap.cpp
 *
 *  Created on: 17.12.2018
 *      Author: aml
 */


#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()
#include <CXmlMap.h>



/**
 *
 * @param argc number of command line parameter
 * @param *argv[] list of parameters
 */
int main(int argc, char *argv[]){


	CXmlMap *map = new CXmlMap();
	int inpDim = 3;
	int outDim = 2;
	int size   = 100;
	map->init(inpDim, outDim, 1,size);
	float inpA[inpDim];
	float outA[outDim];

	try{
		float data = 100.000;
		float delta = 0.001;
		for(int i = 0; i< size; i++){
			for(int a=0; a < inpDim; a++){
				data += delta;
				inpA[a]= data;
			}
			for(int a=0; a < outDim; a++){
				data += delta;
				outA[a]= data;
			}
			map->addLink(inpA,outA);
		}
		map->writeXmlFile((char *)"dummy.xml");
	}catch(...){
		cerr << "Can't read command line parameters, terminate process.\n";
		exit(0);
	};

	exit(0);
};




