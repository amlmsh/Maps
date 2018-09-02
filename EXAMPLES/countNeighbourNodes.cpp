#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include <iostream>
#include <stdexcept>
#include <math.h>

#include "CBaseMap.h"

using namespace std;

XERCES_CPP_NAMESPACE_USE

int main (int argc, char* args[]) {
	CBaseMap map;
	
	int nmbNodes = 1000;
	int interval = 1000;
	int inpDim = 2;
	int outDim = 60;
	map.init(inpDim,outDim,1,1500);
	float* inpNode = new float[inpDim];
	float* outNode = new float[outDim];   
	
	
	srand ( time(NULL) );
	for(int i = 0; i < nmbNodes; i++){
		
		for(int k = 0; k < inpDim; k++){
			inpNode[k] = ((float)rand()) / ((float)RAND_MAX);
	 	 };		
	 	 for(int l = 0; l < outDim; l++){
	 		 outNode[l] = ((float)rand()) / ((float)RAND_MAX);
	 	 };
	 	 map.addLink(inpNode,outNode);
	 };

	 // create a link in the middle
	 for(int k = 0; k < inpDim; k++){
		 inpNode[k] = 0.5;
	 };
	 for(int l = 0; l < outDim; l++){
		 outNode[l] = 0.5;
	 };
	  
	 int equal = true;
	 
	 float firerate;
	 float stimulus;
	 try{
		 int *indexListOut;
		 int *indexListInp;
		 int oldNmbOut = 0;
		 int newNmbOut = 0;
		 int oldNmbInp = 0;
		 int newNmbInp = 0;
		 int c =0 ;
		 
		 float delta = sqrt(outDim) / interval;
		 
		 for(int d = 0; d < interval; d++){
			 stimulus = d*delta;
			 
			 indexListOut = map.getOutputNodeIndicesWithinRegion(outNode,stimulus);
			 if(indexListOut != NULL){
				 
				 firerate = (float) indexListOut[0];
				 firerate = firerate / ((float ) nmbNodes);
				 cout <<  firerate << endl;
				 delete indexListOut;
			  };
		 };	
	 }catch(...){
		 cout << "unknown error. terminate process.\n";
	 };
};

