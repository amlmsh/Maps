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




CXmlMap *mapping_;
CXmlMap *swap_;

bool inputSpace_; // determines the spaces which are 
// going to be mapped into each other



int info(int argc, char* args[]);

int main (int argc, char* args[]) {	

	if(argc != 3){info(argc,args); return (0);};
	
	CXmlMap *map = new CXmlMap();
	CXmlMap *swap = new CXmlMap();
	
	try{
		map->readXmlFileWithoutAttributes(args[1]);
	}catch(...){
		cout << "can't read given map. Terminate progam.\n";
		exit(0);		
	};
	
	int inpDim, outDim, attrDim, size;
	inpDim = map->getInpDim();
	outDim = map->getOutDim();
	attrDim = map->getAttrDim();
	size = map->nmbOfLinks();
	
	swap->init(outDim,inpDim,attrDim,size);
	float *ptrLink;
	for(int i = 0; i < size; i++){
		ptrLink = map->ptrLinkByIndex(i);
		swap->addLink(&(ptrLink[inpDim]),&(ptrLink[0]),&(ptrLink[inpDim+outDim]));
	};
	
	swap->writeXmlFile(args[2]);
	return(0);
};

int info(int argc, char* args[]){
	cout << "\n usage of " << args[0] << "  mapping.xml  swapedMapping.xml : \n\n";
	cout << "  mapping.xml       ...  one mapping \n";
	cout << "  swapedMapping.xml ...  resulting mapping, which swaped input and output space \n\n";
};
