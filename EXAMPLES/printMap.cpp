#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include <iostream>
#include <stdexcept>
#include <math.h>
#include <float.h>

#include "CXmlMap.h"

	
using namespace std;

XERCES_CPP_NAMESPACE_USE

int info(int argc, char* args[]);
float randValue(); // between 0 and 1


int main (int argc, char* args[]) {
	if(argc < 4){info(argc,args); return (0);};

	// define mapping function or the real world
	srand(time(NULL));

	
	CXmlMap *map =  new CXmlMap();
	
	try{
		map->readXmlFileWithoutAttributes(args[1]);
	}catch(...){
		cout << "Can't read mapping from file, terminate!\n\n";
		exit(0);
	};
	
	float prob = ((float) (atof(args[2]))) / 100.0;
	if(prob > 1.0) prob = 1.0;
	if(prob < 0.0) prob = 0.0;
	float offset = 0.0;

	int mapSize = map->nmbOfLinks();
	float *link;
	int dim = map->getInpDim() + map->getOutDim();
	int index;
	for(int i = 0; i < mapSize; i++){
		
		if(randValue() < prob){
			link = map->ptrLinkByIndex(i);			
			for(int j = 3 ; j < argc; j++){
				index = atoi(args[j]);
				if((index > (dim-1)) || (index < 0) ){
					cout << "-0\t"; 
				}else{
					if(index == 1){
						offset = 0.0; //-0.25;
					}else if (index == 2){
						offset = 0.0; //-0.225;
					}else{
						offset = 0.0;
					};
					cout <<  (link[index] + offset) << "\t";
				};
			};
			cout << endl;
		};
	};
	return(0);
};

float randValue(); // between 0 and 1

int info(int argc, char* args[]){
	cout << "\n usage of " << args[0] << " : \n\n";
	cout << args[0] << " map.xml [0...100] n [m] ... \n\n" 
	"\t map.xml   ...  file name map\n" <<
	"\t [0...100] ... probability of printing out the current component \n"
	"\t n         ...  n-th component \n"  <<  
	"\t [m]       ...  index of more components \n\n";		
};

/*
 * returns a random value [0,1]
 * 
 */
float randValue(){
	return (((float)random()) / ((float) RAND_MAX));
};

