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




CXmlMap *mappingA_;
CXmlMap *mappingB_;
CXmlMap *remapping_;

bool inputSpace_; // determines the spaces which are 
// going to be mapped into each other



int info(int argc, char* args[]);

int main (int argc, char* args[]) {	
	if(argc != 5){info(argc,args); return (0);};

	mappingA_ = new CXmlMap();
	mappingB_ = new CXmlMap();
	remapping_ = new CXmlMap();

	if(atoi(args[1]) == 0){
		inputSpace_ = true;
	}else{
		inputSpace_ = false;
	};
	
	
	try{
		mappingA_->readXmlFileWithoutAttributes(args[2]);
		mappingB_->readXmlFileWithoutAttributes(args[3]);
	}catch(...){
		cout << "can't read given maps. Terminate progam.\n";
		exit(0);
	};
	
	int nmbLinksA = mappingA_->nmbOfLinks();
	int nmbLinksB = mappingB_->nmbOfLinks();
	
	
	int inpDim = mappingB_->getInpDim();
	int outDim = mappingB_->getOutDim();
	
	if((inpDim != mappingA_->getInpDim()) || 
			(outDim != mappingA_->getOutDim())){
		cout << "Input and/or output dimension of the two ";
		cout <<	"mappings are different. Treminate program.\n\n";
		exit(0);
	};
	
	float inpNode[inpDim];
	float outNode[outDim];
	float tmpInputNode[inpDim];
	float tmpOutputNode[outDim];
	
    float *absValueA;
	float *absValueB;
	float *diff;
	float *anchor;
	float *ptrLink;
	
	if(inputSpace_){
		absValueA = new float(inpDim);
		absValueB = new float(inpDim);
		diff      = new float(inpDim);
		anchor    = new float(outDim);
		remapping_->init(inpDim,inpDim,1,(nmbLinksA + nmbLinksB));
	}else{
		absValueA = new float(outDim);
		absValueB = new float(outDim);
		diff      = new float(outDim);
		anchor    = new float(inpDim);
		remapping_->init(outDim,outDim,1,(nmbLinksA + nmbLinksB));
	};

	

	for(int i = 0; i < nmbLinksA; i++){
		ptrLink = mappingA_->ptrLinkByIndex(i);
		
		if(inputSpace_){ // difference of the input space
			mappingB_->getClosestOutputNodeLink(&(ptrLink[inpDim]),&(absValueB[0]),tmpOutputNode);
			// get difference between A and B
			for(int k = 0; k < inpDim; k++){
				diff[k] = absValueB[k] - ptrLink[k] ;
			};
			remapping_->addLink(&(ptrLink[0]),&(diff[0]));
		}else{
			mappingB_->getClosestInputNodeLink(&(ptrLink[0]),tmpInputNode,&(absValueB[0]));
			// get difference between A and B
			for(int k = 0; k < outDim; k++){
				diff[k] = absValueB[k] - ptrLink[inpDim + k] ;
			};
			remapping_->addLink(&(diff[0]),&(ptrLink[inpDim]));
		}
	};

	
	for(int i = 0; i < nmbLinksB; i++){
		ptrLink = mappingB_->ptrLinkByIndex(i);
		
		if(inputSpace_){ // difference of the input space
			mappingA_->getClosestOutputNodeLink(&(ptrLink[inpDim]),&(absValueA[0]),tmpOutputNode);
			// get difference between A and B
			for(int k = 0; k < inpDim; k++){
				diff[k] = ptrLink[k] - absValueA[k];
			};
			remapping_->addLink(&(absValueA[0]),&(diff[0]));
		}else{
			mappingA_->getClosestInputNodeLink(&(ptrLink[0]),tmpInputNode,&(absValueA[0]));
			// get difference between A and B
			for(int k = 0; k < outDim; k++){
				diff[k] = ptrLink[inpDim + k] - absValueA[k];
			};
			remapping_->addLink(&(diff[0]),&(absValueA[inpDim]));
		}
	};
	
	
	
	remapping_->writeXmlFile(args[4]);
};





int info(int argc, char* args[]){
	cout << "\n usage of " << args[0] << " 0/1 absMapping1.xml absMapping2.xml diffMapping.xml\n\n";
	cout << "  0/1   ...  input (0) or output space (1) to be added\n";
	cout << "  mapping1.xml ... mapping containing the absolute values \n";
	cout << "  mapping2.xml ...  mapping containing the delta values \n";
	cout << "  diffmapping.xml ...  mapping containing the (abs1 - abs2) values with respect to the input/output space \n \n";
}

