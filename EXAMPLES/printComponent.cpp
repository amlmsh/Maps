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



static int idxA_;
static int idxB_;
static int dim_;
static int size_;

int info(int argc, char* args[]);
int readCommandLine(int argc, char* args[], CXmlMap *map);
int printData(CXmlMap *map);
CXmlMap *deriveData(CXmlMap *map, CXmlMap *data);
void meanError(CXmlMap *dataEst);

int main (int argc, char* args[]) {
	if(info(argc,args) != 0){ exit(0);};
	
	CXmlMap *D3Map = new CXmlMap();
	CXmlMap *D3Estimation = new CXmlMap();
	
	try{
		D3Map->readXmlFile(args[1]);
	}catch(...){
		cout << "cannot read file : \"" << args[1] << "\" terminate process.\n";
		cout.flush();
		exit(0);
	};
	
	try{
		D3Estimation->readXmlFile(args[2]);
	}catch(...){
		cout << "cannot read file : \"" << args[2] << "\" terminate process.\n";
		cout.flush();
		exit(0);
	};

	readCommandLine(argc,args,D3Map);
	
	CXmlMap *D2Estimation = deriveData(D3Map, D3Estimation); 
	
	D2Estimation->writeXmlFile("2dEst.xml");
	
	meanError(D3Estimation);
	meanError(D2Estimation);
	
	//printData(D3Estimation);
	
};

void meanError(CXmlMap *dataEst){
	float *link = NULL;
	float estD, actD, estA, actA;
	int inpDim = dataEst->getInpDim();
	int size = dataEst->nmbOfLinks();
	float maxError = 0.0;
	
	// mean error
	float sumError = 0.0;
	float error =0.0;
	for(int i= 0; i < size; i++){
		link = dataEst->ptrLinkByIndex(i);
		
		estD = link[inpDim];
		estA = link[inpDim + 1];
		actD = link[inpDim + 2];
		actA = link[inpDim + 3];
		
		error = sqrt( ((estD - actD)*(estD - actD)) + ((estA - actA)*(estA - actA)) );
		//error = sqrt( ((estD - actD)*(estD - actD)) );
		//error = sqrt( ((estA - actA)*(estA - actA)) );
		sumError += error;
		if(maxError < error) maxError = error;
		
		//cout << "est/act: " << estD << " / " << actD <<  "   " << fabs(estD - actD) << endl;
		//cout << "est/act: " << estA << " / " << actA <<  "   " << fabs(estA - actA) << endl;
	};
	float meanError = sumError / ((float) size);
	

	// std. deviation
	error = 0.0;
	sumError = 0.0;
	for(int i= 0; i < size; i++){
		link = dataEst->ptrLinkByIndex(i);
		
		estD = link[inpDim];
		estA = link[inpDim + 1];
		actD = link[inpDim + 2];
		actA = link[inpDim + 3];
		error = sqrt( ((estD - actD)*(estD - actD)) + ((estA - actA)*(estA - actA)) );
		//error = sqrt( ((estD - actD)*(estD - actD)) );
		//error = sqrt( ((estA - actA)*(estA - actA)) );
		sumError += (error - meanError)*(error - meanError);
		
	};
	float stdDev = sqrt(sumError / ((float) size));
	
	cout << "error: " << meanError << " +/- " << stdDev << 
		"   max:" << maxError << endl;
};


CXmlMap *deriveData(CXmlMap *map, CXmlMap *data){
	CXmlMap *newData = new CXmlMap();
	CXmlMap *newMap = new CXmlMap();

	// create a new map based only on 2-dimensional 
	// tilt-pan data
	int inpDimMap = map->getInpDim() - 1;
	int outDimMap = map->getOutDim();
	int sizeMap   = map->nmbOfLinks();
	if(inpDimMap != 2) return NULL;
	newMap->init(inpDimMap, outDimMap,1, sizeMap);
	float inpNodeMap[inpDimMap];
	float outNodeMap[outDimMap];
	float *link;
	for(int i = 0; i < sizeMap; i++){
		link = map->ptrLinkByIndex(i);
		inpNodeMap[0] = link[0];
		inpNodeMap[1] = link[1];
		outNodeMap[0] = link[3];
		outNodeMap[1] = link[4];
		newMap->addLink(inpNodeMap, outNodeMap);
	};
	
	
	// create a new estimation data set
	int inpDimData = data->getInpDim() - 1;
	int outDimData = data->getOutDim();
	int sizeData   = data->nmbOfLinks();
	if(inpDimData != 2) return NULL;

	
	newData->init(inpDimData,outDimData,1,sizeData);
	float inpNodeData[inpDimData];
	float outNodeData[outDimData];
	
	float tiltPanNode[inpDimMap];
	for(int i=0; i < sizeData; i++){
		link = data->ptrLinkByIndex(i);
		// get tilt-LeftVerge-configuration
		inpNodeData[0] = link[0];
		inpNodeData[1] = link[1];       
		
		// get new estimation
		newMap->getClosestInputNodeLink(inpNodeData, inpNodeMap,outNodeMap);
		
		// write estimation and actual value into 
		outNodeData[0] = outNodeMap[0];
		outNodeData[1] = outNodeMap[1];
		outNodeData[2] = link[5];
		outNodeData[3] = link[6];
		newData->addLink(inpNodeData,outNodeData);
	};

	delete newMap;
	return newData;
}

int printData(CXmlMap *map){
	
	float *link = NULL;
	float elementA;
	float elementB;
	
	
	for(int i= 0; i < size_; i++){
		link = map->ptrLinkByIndex(i);
		
		elementA = link[idxA_];
		elementB = link[idxB_];
		cout << elementA << "\t" << elementB << endl;

	};
	
}

int readCommandLine(int argc, char* args[], CXmlMap *map){
	
	dim_ = map->getInpDim() + map->getOutDim();
	
	try{
		idxA_ = atoi(args[3]);
	}catch(...){
		cout << "Cannot read all command line parameters, please check.\n";
		cout.flush();
		exit(0);
	};
	
	if(argc == 5){
		try{
			idxB_ = atoi(args[4]);
		}catch(...){
			cout << "Cannot read all command line parameters, please check.\n";
			cout.flush();
			exit(0);
		};		
	};
	
	if((idxA_ < 0) || (idxA_ > (dim_-1)) ||
			(idxB_ < 0) || (idxB_ > (dim_-1)) ){
		cout << "Component index is out of range for the given map. ";
		cout << "Terminate process.\n";
		cout.flush();
		exit(0);		
	};

	
	size_ = map->nmbOfLinks();
	if(size_ < 1){
		cout << "The given map is empty. ";
		cout << "Terminate process.\n";
		cout.flush();
		exit(0);				
	};
	
}

int info(int argc, char* args[]){
	
	if((argc == 5) || (argc == 4)) return (0);
	
	cout << "the use of " << args[0] << " :\n";
	cout << "" << args[0] <<  " <xml-file_Map> <xml-file_Exp> <n> ";
	cout << "\t <xml-file_Map> ... xml file name of the map\n";
	cout << "\t <xml-file_Exp> ... xml file name of the map\n";
	cout << "\t <n>  [<m>]     ... index of the compenent printed.\n";
	return(-1);
};


