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
float realWorldFct(float x, float parameter);
float randValue();
float estimationSingleLink(CXmlMap *m, float v);
CXmlMap *createDummyMap(CXmlMap *m);
void replaceRandomly(CXmlMap *m,float inpNode, float outNode);
void replaceBadEstimation(CXmlMap *m,float inpNode, float outNode);
float estimationRegion(CXmlMap *m, float inpNode, float error);

float testMean(CXmlMap *m,float noise, float shiftParameter_, int testSteps);
float testStd (CXmlMap *m,float noise, float shiftParameter_, float meanErr, int testSteps);


int K_ = 60; // capacity of the mapping, max. nmb. of links
CXmlMap *M_;  // mapping
CXmlMap *tmpM_; 
float fctParam_ = 1.0;
float shiftParameter_ = 0.0;
float noiseLevel_ = 0.01;
int testSteps_ = 100;

// learning trials
int completeTrails_ = 600;
int shiftWorld_   = 300;
int shiftBack_    = 400;



int main (int argc, char* args[]) {

	M_ = new CXmlMap();

	// define mapping function or the real world
	srand(time(NULL));
	
	// init mapping M (empty or learned)
	M_->init(1,1,1,completeTrails_);
	
	int i =0;
	do{
		// select random x and add noise, 
		float x = randValue();
	
	
		// do action A resulting in x_m, in fact adding noise
		float x_m = x +  (noiseLevel_*randValue());
	
	
		// do action B resulting in x_m incl. noise
		if((i < shiftWorld_) || (i > shiftBack_)){
			shiftParameter_ = 0.0;
		}else{
			shiftParameter_ = 0.5;
		};
		
		float y_m = realWorldFct(x_m,shiftParameter_);
		y_m = y_m + (noiseLevel_*randValue()); 
	
	
		// do estimation based on euclidean distance 
		// via mapping M(x_m) --> y_e 
		float y_e = estimationSingleLink(M_,x_m);
		//float y_e = estimationRegion(M_,x_m,noiseLevel_);
	
		// compare estimation  |y_e - x_m|
		float error = (y_e - y_m); 
		error = fabs(error);
			
		// add or not add link (x_m,y_m)
		if(error > (noiseLevel_)){
			if(M_->nmbOfLinks() > K_){
				// no new link added
				
				// replace a link randomly selected
				//replaceRandomly(M_,x_m,y_m);
				
				// replace the link with the bad estimation
				replaceBadEstimation(M_, x_m,y_m);				
			}else{
				M_->addLink(&x_m,&y_m);
			};
		}else{
			if(M_->nmbOfLinks() > K_){
				replaceRandomly(M_,x_m,y_m);
			}else{
				M_->addLink(&x_m,&y_m);				
			}
		}
		
		// make test and print the results mean and std deviation
		float meanErr;
		float stdErr;

		meanErr = testMean(M_,noiseLevel_,shiftParameter_, testSteps_);
		stdErr  = testStd(M_,noiseLevel_,shiftParameter_,meanErr, testSteps_);
		
		cout << meanErr << "\t" << stdErr << endl;
		
	}while(i++ < completeTrails_);
	

	
	//write mapping and dummy mapping for visualization
	
	M_->writeXmlFile("1dMapping.xml");
	// create dummy map
	CXmlMap *D = createDummyMap(M_);
	float inpNode[2];
	float outNode[2];
	inpNode[0] = 0.0;
	inpNode[1] = 0.0;
	outNode[0] = 0.0;
	outNode[1] = 0.0;
	D->addLink(inpNode,outNode);
	inpNode[0] = 1.0;
	inpNode[1] = 1.0;
	outNode[0] = 1.0;
	outNode[1] = 1.0;
	D->addLink(inpNode,outNode);
	D->writeXmlFile("1dTo2dMapping.xml");
	
	M_->clean();
	D->clean();
	delete M_;
	delete D;
	
};

float testMean(CXmlMap *m,float noise, float shiftP, int testSteps){
	int i =0;
	float y_e;
	float errSum = 0.0;
	do{
		// select random x and add noise, 
		float x = randValue();
		
		// do action A resulting in x_m, in fact adding noise
		float x_m = x +  (noise*randValue());
	
	
		// do action B resulting in x_m incl. noise	
		float y_m = realWorldFct(x_m,shiftP);
		y_m = y_m + (noise*randValue()); 
	
	
		// do estimation based on euclidean distance 
		// via mapping M(x_m) --> y_e 
		y_e = estimationSingleLink(M_,x_m);
		
		// compare estimation  |y_e - x_m|
		float error = (y_e - y_m); 
		
		errSum += error;
	}while(i++ < testSteps);
	return (((float) errSum) / ((float) testSteps) );
};

float testStd (CXmlMap *m,float noise, float shiftP, float meanErr, int testSteps){
	int i =0;
	float y_e;
	float errDev = 0.0;
	do{
		// select random x and add noise, 
		float x = randValue();
		
		// do action A resulting in x_m, in fact adding noise
		float x_m = x +  (noise*randValue());
	
	
		// do action B resulting in x_m incl. noise	
		float y_m = realWorldFct(x_m,shiftP);
		y_m = y_m + (noise*randValue()); 
	
	
		// do estimation based on euclidean distance 
		// via mapping M(x_m) --> y_e 
		y_e = estimationSingleLink(M_,x_m);
		
		// compare estimation  |y_e - x_m|
		float error = (y_e - y_m); 
		
		errDev += ((error - meanErr)*(error - meanErr));
	}while(i++ < testSteps);
	return ( sqrt((float) errDev) / ((float) testSteps));
};




void replaceBadEstimation(CXmlMap *m,float inpNode, float outNode){
	// find the node closest to inpNode
	int nmbLinks = m->nmbOfLinks();
	float *link;
	
	int index = 0;
	link = m->ptrLinkByIndex(index);
	float dist = fabs(link[0] - inpNode);
	int i = 1;
	float tmpDist;
	do{
		link = m->ptrLinkByIndex(i);
		tmpDist = fabs(link[0] - inpNode);
		
		if(tmpDist < dist){
			index = i;
			dist = tmpDist;
		};
		i++;
	}while(i < nmbLinks);
	
	
	// replace to link with closest input node 
	// because it has coused the bad estimation
	link = m->ptrLinkByIndex(index);
	*(&(link[0])) = inpNode;
	*(&(link[1])) = outNode;
	return;
};

/*
 * 
 * Assume a mapping having 1-dim input and 
 * output space; replace a randomly selected
 * link in the mapping by the given values
 */
void replaceRandomly(CXmlMap *m,float inpNode, float outNode){
	int nmbLinks = m->nmbOfLinks();
	int randIdx = (int)(nmbLinks*randValue());
	float *link = m->ptrLinkByIndex(randIdx);
	*(&(link[0])) = inpNode;
	*(&(link[1])) = outNode;
	return;
}

CXmlMap *createDummyMap(CXmlMap *m){
	CXmlMap *tmp = new CXmlMap();
	int count = m->nmbOfLinks();
	tmp->init(2,2,1,count);
	
	float inpNode[2];
	float outNode[2];
	float *linkValues = NULL;
	
	for(int i = 0; i < count;i++){
		linkValues = m->ptrLinkByIndex(i);
		inpNode[0] = linkValues[0];
		inpNode[1] = linkValues[0];
		outNode[0] = linkValues[1];
		outNode[1] = linkValues[1];
		tmp->addLink(inpNode,outNode);
	};
	return tmp;
};


float estimationSingleLink(CXmlMap *m, float inpNode){
	float inpNodeStorage;
	float outNodeStorage;
	m->getClosestInputNodeLink(&inpNode,&inpNodeStorage,&outNodeStorage);
	return outNodeStorage;
}

float estimationRegion(CXmlMap *m, float inpNode, float error){
	float inpNodeStorage;
	float outNodeStorage;
	int *indexList;
	
	indexList = m->getInputNodeIndicesWithinRegion(&inpNode,error);
	if(indexList == NULL){
		return (estimationSingleLink(m,inpNode));
	};
	
	int nmbLinks = indexList[0];
	int currIdx;
	float *link;
	float sumOutNodes = 0.0;
	if(nmbLinks>0){
		int i = 0;
		do{
			currIdx = indexList[i+1];
			i++;
			link = m->ptrLinkByIndex(currIdx);
			sumOutNodes += link[1];
		}while(i < nmbLinks);
		return (sumOutNodes / nmbLinks);
	}else{
		return (estimationSingleLink(m,inpNode));
	};
}

/*
 * Any function with x in [0,1]
 * and f(x) in [0,1]
 */
float realWorldFct(float x, float parameter){
	float y = (x / 2.0) + parameter;
	if(y < 0) return 0.0;
	if(y > 1.0) return 1.0;
	return y;
}

/*
 * returns a random value [0,1]
 * 
 */
float randValue(){
	return (((float)random()) / ((float) RAND_MAX));
};


int info(int argc, char* args[]){
};

