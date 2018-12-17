/*
 * fillMap.cpp
 *
 *  Created on: 14.12.2018
 *      Author: aml
 */

#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>


#include <algorithm>
#include <iterator>
#include <cassert>


#include "CXmlMap.h"

XERCES_CPP_NAMESPACE_USE

using namespace std;

void info(int argc, char* args[]);
void storeValues(string line, int inpDim, int outDim, float *inpA, float *outA);


int main(int argc, char* args[]){
	if(argc != 5){info(argc,args); exit (0);};

	int inpDim = atoi(args[1]);
	int outDim = atoi(args[2]);

	if((inpDim < 1) || (outDim < 1)){
		cout << "Input / Output dimension must not be smaller 1\n";
		exit(0);
	}

	cout << "create map of input/output dimension:" << inpDim << "/" << outDim << endl;
	float inpA[inpDim];
	float outA[outDim];


	try{
		CXmlMap *map = new CXmlMap();
		map->init(inpDim, outDim,1, 1000);

		ifstream srcData(args[3]);
		if (srcData.is_open()) {
			std::string line;
			while (getline(srcData, line)) {
				storeValues(line, inpDim, outDim, inpA, outA);
				map->addLink(inpA, outA);
			}
			srcData.close();
		}

		map->writeXmlFile(args[4]);
	}catch(IMapException *e){
		cout << "Error, terminate: " << e->getMsg() << endl;
	}catch(...){
		cout << "unknown error, terminate.\n";
	}
	return 0;
}


void storeValues(string line, int inpDim, int outDim, float *inpA, float *outA){
	// If possible, always prefer std::vector to naked array
	vector<float> values;

	// Build an istream that holds the input string
	istringstream iss(line);

	 // Iterate over the istream, using >> to grab floats
	 // and push_back to store them in the vector
	 copy(istream_iterator<float>(iss),istream_iterator<float>(),back_inserter(values));

	 // collect data
	 int detectedFloats = values.size();
	 if (detectedFloats !=  (inpDim + outDim)){
		stringstream s1, s2;
		s1 << detectedFloats;
		s2 << (inpDim + outDim);
		string msg("conflicting dimensions while reading values from  line.  We have \n");
		msg += s1.str() + " reads and while expected dimension is ";
		msg += s2.str() + "\n";
		throw new IMapException(msg,-1);
	}

	for(int idx = 0; idx < inpDim; idx++){
		inpA[idx] = values[idx];
	}

	for(int idx = inpDim; idx < (inpDim + outDim); idx++){
		outA[idx-inpDim] = values[idx];
	}

	return;
}


void info(int argc, char* args[]){
	cout << "\n usage of " << args[0] << " inpDim  outDim  source.txt resultMap.xml\n\n";
	cout << "  inpDim        ...  dimension of input space (>0)\n";
	cout << "  outDIm        ...  dimension of outputspace space (>0)\n";
	cout << "  source.txt    ...  text file containing the data the map shall be filled with \n";
	cout << "  resultMap.xml ... name of the xml-file the map is stored in\n \n";
	return;
}
