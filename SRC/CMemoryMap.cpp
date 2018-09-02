#include "CMemoryMap.h"


CMemoryMap::CMemoryMap(){
	map_ = NULL;
	activations_ = NULL;
	mapDim_ =0;
	nmbLinks_ = 0;
};


CMemoryMap::~CMemoryMap(){
	if(activations_ != NULL){
		delete activations_;
	};
};

void CMemoryMap::initMap(CBaseMap *map) throw (IMapException){
	if(map == NULL){
		throw (IMapException(string("Map is NULL pointer.\n"), -1));
	};
	
	nmbLinks_ = map->nmbOfLinks(); 
	if( nmbLinks_ <= 0){
		nmbLinks_ = 0;
		throw (IMapException(string("Map isn't initialized yet or still empty.\n"), -1));
	};
	
	if(map->getInpDim() != map->getOutDim()){
		nmbLinks_ = 0;
		throw (IMapException(string("Input and output space of the init map are different.\n"),-1));
	}

	if((map->getInpDim() < 1) || 
			(map->getOutDim() < 1)){
		nmbLinks_ = 0;
		throw (IMapException(string("Input or output space of the init map are less than 1.\n"),-1));
	}

	try{
		activations_ = new unsigned int[nmbLinks_];
	}catch(bad_alloc e){
		activations_ = NULL;
		nmbLinks_ = 0;
		throw (IMapException(string("Cannot allocate memory to store activations.\n"),-1));
	}catch(...){
		activations_ = NULL;
		nmbLinks_ = 0;
		throw (IMapException(string("Unknown error, no memory to store activations.\n"),-1));
	};
	
	map_ = map;
	mapDim_ = map->getInpDim();
	nmbLinks_ = map_->nmbOfLinks();
};

void CMemoryMap::deinitMap(){
	map_ = NULL;
	nmbLinks_ = 0;
	mapDim_ = 0;
	delete activations_;
	activations_ = NULL;
};



int CMemoryMap::getDim(){
	return (mapDim_);
};

int CMemoryMap::getNmbLinks(){
	return (nmbLinks_);
};

void CMemoryMap::updateActivationWithClosestNodes(int nmbInputNodes, float *inputNodes) 
throw (IMapException){
	if(activations_ == NULL){
		throw (IMapException(string("Memory Map not initialized yet."),-1));
	};
	// clear activation values 
	for(int i = 0; i < nmbLinks_;i++){activations_[i] = 0;};
	if((nmbInputNodes == 0) || (inputNodes == NULL)){return;};
	
	// replace given input nodes by closest node,
	// which is the same, if node exists in the input layer
	float newInpNode[mapDim_];
	float outNode[mapDim_];
	int baseIdx; 
	for(int i = 0; i < nmbInputNodes; i++){
		baseIdx = i*mapDim_;
		try{
			map_->getClosestInputNodeLink (&(inputNodes[baseIdx]), newInpNode, outNode);
			// replace with values of the closest node
			for(int k = 0; k < mapDim_;k++){
				inputNodes[baseIdx + k] = newInpNode[k];
			};
		}catch(IMapException e){throw e;
		}catch(...){
			throw (IMapException(string("Cannot do update: unknown error while getting clostest node.\n"),-1));
		};
	};
	
	try{
		updateActivation(nmbInputNodes, inputNodes);
	}catch(IMapException e){throw e;
	}catch(...){
		throw (IMapException(string("Cannot do update: unknown error.\n"),-1));
	};
};

void CMemoryMap::updateActivation(int nmbInputNodes, float *inputNodes) 
throw (IMapException){
	if(activations_ == NULL){
		throw (IMapException(string("Memory Map not initialized yet."),-1));
	};
	
	// clear activation values 
	for(int i = 0; i < nmbLinks_;i++){
		activations_[i] = 0;
	}
	
	if(nmbInputNodes < 1) return;
	int *resultIdxList; 
	int baseIdx;
	for(int i = 0; i < nmbInputNodes; i++){
		baseIdx = mapDim_ * i;
		resultIdxList = 
			map_->getInputNodeIndicesWithinRegion(&(inputNodes[baseIdx]), 0.0);


		
		if(resultIdxList == NULL) continue; 
		if(resultIdxList[0] == 0){
			delete (resultIdxList);
			continue;
		}
		// otherwise, update activations
		for(int j = 0; j < resultIdxList[0]; j++){
			activations_[resultIdxList[j+1]] += 1;
		}
		delete (resultIdxList);
	}
};


void CMemoryMap::getAllActivationsValues(unsigned int* valuesStorage)
throw (IMapException){
	if(activations_ == NULL){
		throw (IMapException(string("Memory Map not initialized yet."),-1));
	};

	for(int i = 0; i < nmbLinks_; i++){
		valuesStorage[i] = activations_[i];
	}
};


long int CMemoryMap::sumActivation(){
	if(activations_ == NULL){
		return 0;
	};

	long int sum = 0;
	for(int i = 0; i < nmbLinks_; i++){
		sum += activations_[i];
	}
	return sum;
};


float *CMemoryMap::getActiveNodes(int *nmbOutputNodes, unsigned int* activationValues) 
throw (IMapException){
	if(activations_ == NULL){
		throw (IMapException(string("Memory Map not initialized yet."),-1));
	};

	// count nodes with activation value larger than zero
	int count = 0;
	for(int k=0; k<nmbLinks_; k++){
		if(activations_[k] > 0) count++;
	};
	
	*nmbOutputNodes = count;
	float *outputNodes;
	
	try{
		outputNodes = new float[count*mapDim_];
	}catch(bad_alloc e){
		throw (IMapException(string("Cannot allocate memory to store nodes.\n"),-1));
	}catch(...){
		throw (IMapException(string("Unknown error, no memory to store nodes.\n"),-1));
	};
	
	int baseIdx;
	float *ptrLink;
	float value;
	int outputNodeIdx = 0;
	int outputNodeBaseIdx;
	for(int i=0; i<nmbLinks_; i++){
		
		if(activations_[i] > 0){
			outputNodeBaseIdx = mapDim_*outputNodeIdx;
			// get node value
			ptrLink = map_->ptrLinkByIndex(i);
			// store only output node data
			for(int j = 0; j < mapDim_; j++){
				value = ptrLink[mapDim_ + j];
				outputNodes[outputNodeBaseIdx + j] = value;
			};	
		};
		
	};
	
	return outputNodes;
};


