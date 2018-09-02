#include "CBaseMap.h"
#include <iostream>
#include <sstream>
#include <math.h>

CBaseMap::CBaseMap(){
	listOfLinks_ = NULL;
	maxNmbLinks_ = 0;
	indexLastLink_ = -1;
	inpDim_ = 0;
	outDim_ = 0;
	attrDim_ = 0;
};

CBaseMap::~CBaseMap(){
	if(maxNmbLinks_ != 0){
		delete [] listOfLinks_;
	};
};

void CBaseMap::init(int inpDim, int outDim, int attrDim, int initSize) 
throw (IMapException){
	if(maxNmbLinks_ != 0){
		string msg("Map is already initalized!\n");
		throw IMapException(msg,-1);
	};
	
	if((inpDim < 1) || (outDim < 1)){
		string msg("Input and/or output space dimension is less than 1.\n");
		throw IMapException(msg,-2);
	};

	if(attrDim < 1){
		string msg("Dimension of the attribute value is less than 1.\n");
		throw IMapException(msg,-2);
	};
	
	if(initSize < 1){
		string msg("Init size is less than 1.\n");
		throw IMapException(msg,-3);
	};
	

	
	try{
		listOfLinks_ = new float[(inpDim + outDim + attrDim)*initSize];
	}catch(bad_alloc e){
		string msg("1: List is full, and unable to get additional storage. No new link added.\n");
		throw(IMapException(msg,-11));			
	}catch(...){
		string msg("Cannot get enought memory to initalize map.\n");
		throw IMapException(msg,-4);
	};
	
	if(listOfLinks_ == NULL){
		string msg("Cannot get enought memory to initalize map.\n");
		throw IMapException(msg,-4);
	};
	
	initSize_ = initSize;
	maxNmbLinks_ = initSize;
	inpDim_ = inpDim;
	outDim_ = outDim;
	attrDim_ = attrDim;
	indexLastLink_ = -1;	
};

void CBaseMap::deinit(){
	maxNmbLinks_ = 0;
	delete [] listOfLinks_;
	listOfLinks_ = NULL;
	indexLastLink_ = -1;
}

void CBaseMap::clean(){
	indexLastLink_ = -1;
}

void CBaseMap::addLink(float *inpNode, float *outNode) throw (IMapException){
	if(attrDim_ < 1){
		addLink(inpNode,outNode,NULL);
	}else{
		float *attrValues = new float[attrDim_];
		for(int i = 0; i < attrDim_;i++){
			attrValues[i] = 0.0;
		};
		addLink(inpNode,outNode,attrValues);
		delete [] attrValues;
	};
};


void CBaseMap::addLink(float *inpNode, float *outNode, float *linkAttributes)
throw (IMapException){
	if((maxNmbLinks_ == 0) || 
		(listOfLinks_ == NULL)){
			string msg("Map not initialized yet, cannot  store links.\n");
			throw(IMapException(msg,-6));
	};
		
	if((indexLastLink_ + 1) == maxNmbLinks_){
		// list full create new storage	
		float *fullList = listOfLinks_;
		int    oldSize = maxNmbLinks_*(inpDim_+outDim_+attrDim_);
		int    additionalSize = initSize_*(inpDim_ + outDim_ + attrDim_);
		float *newList;
		
		try{
			newList = new float[oldSize + additionalSize];
		}catch(bad_alloc e){
			string msg("1: List is full, and unable to get additional storage. No new link added.\n");
			throw(IMapException(msg,-11));			
		}catch(...){
			string msg("2: List is full, and unable to get additional storage. No new link added.\n");
			throw(IMapException(msg,-11));			
		}
		

		
		// recopy elements	
		reCopy(newList,fullList,(maxNmbLinks_*(inpDim_ + outDim_ + attrDim_)));
		maxNmbLinks_ = maxNmbLinks_ + initSize_;
		listOfLinks_ = newList;
		delete [] fullList;

	};
	
	// add new link
	indexLastLink_++;
	int baseIdx = (outDim_ + inpDim_ + attrDim_)*(indexLastLink_);
	for(int i = 0; i < inpDim_; i++){
		listOfLinks_[baseIdx + i] = inpNode[i]; 
	};
	for(int i = 0; i < outDim_; i++){
		listOfLinks_[baseIdx + inpDim_ + i] = outNode[i];
	};
	for(int i = 0; i < attrDim_; i++){
		listOfLinks_[baseIdx + inpDim_ + outDim_ + i] = linkAttributes[i];
	};
	
}

void CBaseMap::removeLink(int nodeIdx){
	if(nodeIdx < 0) return;
	if(nodeIdx > indexLastLink_) return;
	
	if(nmbOfLinks() == 1){
		clean();
		return;
	}
	
	// swap values of i-th link with 
	// last link in this list
	float *tmpLinkStorage = new float[inpDim_ + outDim_ + attrDim_];
	float *ptrLastLink = &(listOfLinks_[(inpDim_ + outDim_ + attrDim_)*indexLastLink_]);
	float *ptrIthLink = &(listOfLinks_[(inpDim_ + outDim_ + attrDim_)*nodeIdx]);
	
	for(int i = 0; i < (inpDim_ + outDim_ + attrDim_);i++){
		tmpLinkStorage[i] = ptrLastLink[i];	
	};

	for(int i = 0; i < (inpDim_ + outDim_ + attrDim_);i++){
		ptrLastLink[i] =  ptrIthLink[i];	
	};
	for(int i = 0; i < (inpDim_ + outDim_ + attrDim_);i++){
		ptrIthLink[i] = tmpLinkStorage[i];	
	};

	
	// remove last link 
	indexLastLink_ = indexLastLink_ - 1;
};



void CBaseMap::reCopy(float *destination, float *source, int nmb){
	for(int i = 0; i < nmb; i++){
		destination[i] = source[i];
	}
}

int  CBaseMap::nmbOfLinks(){
	return (indexLastLink_ + 1);  
}

float CBaseMap::euclidDist(float *inp1, float *inp2, int nmb){
	float d = 0.0;
	for(int i = 0; i<nmb;i++){
		d = d + (inp1[i] - inp2[i])*(inp1[i] - inp2[i]);
	}
	return (sqrt(d));
}

void CBaseMap::getClosestOutputNodeLink(float* outNodeValues,  float* inpNodeStorage, float* outNodeStorage) 
throw (IMapException){
	float *currOutNode = NULL; 
	float *currInpNode = NULL;
	float currDistance;
	float minDistance;
	
	int nmb = indexLastLink_ + 1;
	int delta = (inpDim_ + outDim_ + attrDim_);
	int baseIdx;
	for(int i = 0; i < nmb; i++){
		baseIdx = (delta*i);
		currOutNode = &(listOfLinks_[baseIdx + inpDim_]);
		currDistance = euclidDist(currOutNode,outNodeValues,outDim_);
		
		if((i == 0) || (currDistance < minDistance)){
			minDistance = currDistance; 
			for(int k = 0; k < inpDim_; k++){
				inpNodeStorage[k] = listOfLinks_[baseIdx + k];
			};
			for(int l = 0; l < outDim_;l++){
				outNodeStorage[l] = listOfLinks_[baseIdx + inpDim_ + l];
			};
		};
	};
}


void CBaseMap::getOutputNode(float* inpNode, float* corrOutNodeStorage) 
throw (IMapException){
	bool found = false;
	float *currInpNode = NULL;
	float distance;
	
	int nmb = indexLastLink_ + 1;
	int delta = (inpDim_ + outDim_ + attrDim_);
	int baseIdx;
	for(int i = 0; i < nmb; i++){
		baseIdx = delta*i;
		currInpNode = &(listOfLinks_[baseIdx]);
		distance = euclidDist(currInpNode,inpNode,inpDim_);
		if(distance == 0){
			// found node, get output node values
			for(int j = 0; j < outDim_; j++){
				corrOutNodeStorage[j] = listOfLinks_[baseIdx + inpDim_ + j];
			};
			found = true;
			break;
		};
	};
	if(!found){
		string msg("Given input doesn't exist, resulting output node value undefined.\n");
		throw (IMapException(msg,-8));
	}
}

void CBaseMap::getClosestInputNodeLink (float* inpNodeValues, float* inpNodeStorage,  float* outNodeStorage) 
throw (IMapException){
	float *currOutNode = NULL; 
	float *currInpNode = NULL;
	float currDistance;
	float minDistance;
	
	int nmb = indexLastLink_ + 1;
	int delta = (inpDim_ + outDim_ + attrDim_);
	int baseIdx;
	for(int i = 0; i < nmb; i++){
		baseIdx = delta*i;
		currInpNode = &(listOfLinks_[baseIdx]);
		currDistance = euclidDist(currInpNode,inpNodeValues,inpDim_);
		
		if((i == 0) || (currDistance < minDistance)){
			minDistance = currDistance; 
			for(int k = 0; k < inpDim_; k++){
				inpNodeStorage[k] = listOfLinks_[baseIdx + k];
			};
			for(int l = 0; l < outDim_;l++){
				outNodeStorage[l] = listOfLinks_[baseIdx + inpDim_ + l];
			};
		};
	};
};


void CBaseMap::getInputNode (float* outNode, float* inpNodeStorage) 
throw (IMapException){
	
	if((maxNmbLinks_ < 1) || (listOfLinks_ == NULL)){
		string msg("Map is not initalized yet, contains no links.\n");
		throw (IMapException(msg,-10));
	};
	if(indexLastLink_ < 0){
		string msg("Map is empty, no links stored so far.");
		throw (IMapException(msg,-11));
	};
	
	
	bool found = false;
	float *currOutNode = NULL; 
	float distance;
	
	int nmb = indexLastLink_ + 1;
	int delta = (inpDim_ + outDim_ + attrDim_);
	int baseIdx;
	for(int i = 0; i < nmb; i++){
		baseIdx = (delta*i) + inpDim_;
		currOutNode = &(listOfLinks_[baseIdx]);
		distance = euclidDist(currOutNode,outNode,outDim_);
		if(distance == 0){
			// found node, get output node values
			for(int j = 0; j < inpDim_; j++){
				inpNodeStorage[j] = listOfLinks_[baseIdx - inpDim_ + j];
			};
			found = true;
			break;
		};
	};
	if(!found){
		string msg("Given output doesn't exist, resulting input node value undefined.\n");
		throw (IMapException(msg,-9));
	};
}


float* CBaseMap::ptrLinkByIndex(int i){
	if(i < 0) return NULL;
	if(i > indexLastLink_) return NULL;
	return (&(listOfLinks_[i*(inpDim_ + outDim_ + attrDim_)]));
}




int *CBaseMap::getOutputNodeIndicesWithinRegion(float *outNodeValues, float radius){
	int *resultList;
	if((maxNmbLinks_ < 1) || (listOfLinks_ == NULL)){return NULL;};
	if(indexLastLink_ < 0){return NULL;};	
	try{resultList = new int[indexLastLink_+1];	}
	catch(bad_alloc e){return NULL;	}
	catch(...){return NULL;};
	
	int indexNodeList = 1;
	float *currOutNode = NULL;
	float distance;
	int nmb = indexLastLink_ + 1;
	int delta = (inpDim_ + outDim_ + attrDim_);
	int baseIdx;
	for(int i = 0; i < nmb; i++){
		baseIdx = (delta*i) + inpDim_;
		currOutNode = &(listOfLinks_[baseIdx]);
		distance = euclidDist(currOutNode,outNodeValues,outDim_);
		if(distance <= radius){
			// found output node within this region			
			resultList[indexNodeList] = i; // index
			indexNodeList++;
		};
	};
	resultList[0] = indexNodeList - 1; // number of indices in the list
	return (resultList);
}

int *CBaseMap::getInputNodeIndicesWithinRegion(float *inpNodeValues, float radius){
	int *resultList;
	if((maxNmbLinks_ < 1) || (listOfLinks_ == NULL)){return NULL;};
	if(indexLastLink_ < 0){return NULL;};	
	try{resultList = new int[indexLastLink_+2];	}
	catch(bad_alloc e){return NULL;	}
	catch(...){return NULL;};
	
	int indexNodeList = 1;
	float *currInpNode = NULL;
	float distance;
	int nmb = indexLastLink_ + 1;
	int delta = (inpDim_ + outDim_ + attrDim_);
	int baseIdx;
	for(int i = 0; i < nmb; i++){
		baseIdx = (delta*i);
		currInpNode = &(listOfLinks_[baseIdx]);
		distance = euclidDist(currInpNode,inpNodeValues,inpDim_);
		if(distance <= radius){
			// found output node within this region			
			resultList[indexNodeList] = i; // index
			indexNodeList++;
		};
	};
	resultList[0] = indexNodeList - 1; // number of indices in the list
	return (resultList);
};


/**
 * 
 * 
 * 
 */
int CBaseMap::getInpDim(){
	if(listOfLinks_ == NULL){
		return(0);
	}else{
		return(inpDim_);
	};
}

/**
 * 
 * 
 * 
 */
int CBaseMap::getOutDim(){
	if(listOfLinks_ == NULL){
		return (0);
	}else{
		return(outDim_);
	};
}

int CBaseMap::getAttrDim(){
	if(listOfLinks_ == NULL){
		return (-1);
	}else{
		return(attrDim_);
	};
}


/*
 * 
 * IMapException implementation
 * 
 * 
 */
IMapException::IMapException(string s, int code){msg_ = string(s);code_ = code;};
IMapException::~IMapException(){};
string IMapException::getMsg(){return msg_;};
string IMapException::getSupport(){return support_;};
int IMapException::getCode(){return code_;};


