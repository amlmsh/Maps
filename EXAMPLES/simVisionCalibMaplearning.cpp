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


// extension class for arging nodes
class CAgingMapping : public CXmlMap {	
public :
	CAgingMapping() : CXmlMap() {;};
	~CAgingMapping() {;};
	
	void incAgeForAll(int add){
		// assume 1st component of the attribute values
		// is the age and (inpDim + outDim) = 5
		float *ptrLink;
		for(int i = 0; i < nmbOfLinks();i++){
			ptrLink = ptrLinkByIndex(i);
			*(&(ptrLink[inpDim_ + outDim_])) += add;
		};
	};
	void setAgeForAll(int age){
		if(age < 0) age = 0;
		// assume 1st component of the attribute values
		// is the age and (inpDim + outDim) = 5
		float *ptrLink;
		for(int i = 0; i < nmbOfLinks();i++){
			ptrLink = ptrLinkByIndex(i);
			*(&(ptrLink[inpDim_ + outDim_])) = age;
		};
	};
	
	int getHighestAge(){
		int oldestAge = 0;
		float *ptrLink;
		for(int i = 0; i < nmbOfLinks();i++){
			ptrLink = ptrLinkByIndex(i);
			if(ptrLink[inpDim_ + outDim_] > oldestAge){
				oldestAge = ptrLink[inpDim_ + outDim_];
			};
		};
		return oldestAge;
	};
	
	float averageAge(){
		int age = 0;
		int size = nmbOfLinks();
		float *ptrLink;
		for(int i = 0; i < size;i++){
			ptrLink = ptrLinkByIndex(i);
			age = age + ptrLink[inpDim_ + outDim_];
		};
		return (((float) age) / ((float) size));
	};
	
};




// constants and global stuff
CXmlMap *events_;
CXmlMap *eventsTests_;
CXmlMap *fixedMap_;
CAgingMapping *adaptingCalibMap_;

// variables

float noiseLevel_ = 8.5;
float K_ = 30;

int steps_ = 300;



// domain limits
// for learning
float aMin_ = -2.3;
float aMax_ = 2.3; 
float dMin_ = 30;
float dMax_ = 60;

//  for testing
float aMinT_ = -2.4;
float aMaxT_ = 2.4; 
float dMinT_ = 30;
float dMaxT_ = 60;
float dStep_ = 100;
float aStep_ = 100;



// functions
int info(int argc, char* args[]);
float meanErrTableDist(CAgingMapping *mapping,CXmlMap *fixedMap, CXmlMap *testSamples, float dMin, float dMax, float aMin, float aMax);
float stdDevTableDist(CAgingMapping *mapping,CXmlMap *fixedMap, CXmlMap *testSamples, float meanErr, float dMin, float dMax, float aMin, float aMax);
float distanceOnTable(float angle1, float dist1, float angle2, float dist2);
void replaceBadEstimation(CAgingMapping *m, float *inpNode, float *outNode);
void makeYoungGoodEstimation(CAgingMapping *m, float *inpNode, float *outNode);
void replaceRandomly(CAgingMapping *m, float *inpNode, float *outNode);
void removeVeryOldLinks(CAgingMapping *m, int capacity);

float qf(float x, float a,float b, float c);

int main (int argc, char* args[]) {
	if(argc != 5){info(argc,args); return (0);};

	
	try{
		events_          = new CXmlMap();
		eventsTests_     = new CXmlMap();
		fixedMap_         = new CXmlMap();
		adaptingCalibMap_ = new CAgingMapping();

		fixedMap_    ->readXmlFileWithoutAttributes(args[1]);
		//adaptingCalibMap_->readXmlFileWithoutAttributes(args[2]);
		adaptingCalibMap_->init(3,3,1,500);
		events_     ->readXmlFileWithoutAttributes(args[3]);
		eventsTests_->readXmlFileWithoutAttributes(args[4]);
	}catch(...){
		cout << "Can't read mappings from files, terminate!\n\n";
		exit(0);
	};
	
	float   *link;
	
	float actTabDist,  actTabAngle, actTilt, actVergeL, actVergeR;
	float estTabAngle, estTabDist,  estTilt, estVergeL, estVergeR;
	float estTiltDelta, estVergeLDelta, estVergeRDelta;
	float currTiltDelta, currVergeLDelta, currVergeRDelta;
	float posError, meanError, stdError;
	float *tableNode    = new float(2);
	float *tableNodeTmp = new float(2);
	float *visNode      = new float(3);
	float *visNodeTmp   = new float(3); 
	float *adaptInpNode = new float(3);
	float *adaptOutNode = new float(3);
	
	
	int eventIdx = 0;
	int validSteps = 0;
	while(validSteps < steps_){
		
		// get data actual data tilt, verge L and R and 
		// table distance and angle
		if(eventIdx < events_->nmbOfLinks()){
			eventIdx++;
		}else{
			eventIdx = 0;
		};
			
		link = events_->ptrLinkByIndex(eventIdx);
		if(link == NULL) continue;
		
		actTilt      = link[0];
		actVergeL    = link[1];
		actVergeR    = link[2];
		actTabDist   = link[3];
		actTabAngle  = link[4];
		// test limits
		if((actTabDist < dMin_) || (actTabDist > dMax_)){
			continue;
		};
		if((actTabAngle < aMin_) || (actTabAngle > aMax_)){
			continue;
		};
		validSteps++;

		// 1. estimate shift of visual space
		adaptingCalibMap_->getClosestInputNodeLink(&(link[0]),&(adaptInpNode[0]),&(adaptOutNode[0]));
		estTiltDelta   = adaptOutNode[0];
		estVergeLDelta = adaptOutNode[1];
		estVergeRDelta = adaptOutNode[2];
		
		// estimation via calculation
		// quadratic
		//estTiltDelta   = qf(actTilt,-1.408,-1.1651,-0.2385); 
		//estVergeLDelta = qf(actVergeL, 0.527,-0.208,-0.246);
		//estVergeRDelta = qf(actVergeR, 0.4776,-0.0838,-0.268); 
		//linear 
		//estTiltDelta   = qf(actTilt,0.0  ,-0.0529,-0.0212); 
		//estVergeLDelta = qf(actVergeL,0.0, 0.073 ,-0.2569);
		//estVergeRDelta = qf(actVergeR,0.0,-0.0187,-0.243);
		//avg 
		//estTiltDelta   = qf(actTilt,  0.0,0.0,-0.0006); 
		//estVergeLDelta = qf(actVergeL,0.0,0.0,-0.2359);
		//estVergeRDelta = qf(actVergeR,0.0,0.0,-0.2442);
		//none 
		//estTiltDelta   = qf(actTilt,  0.0,0.0,0.0); 
		//estVergeLDelta = qf(actVergeL,0.0,0.0,0.0);
		//estVergeRDelta = qf(actVergeR,0.0,0.0,0.0);
		
		
		estTilt   = actTilt   + estTiltDelta;
		estVergeL = actVergeL + estVergeLDelta;
		estVergeR = actVergeR + estVergeRDelta;
		
		// 2. estimate table position angle and distance via fixed map
		visNodeTmp[0] = estTilt; 
		visNodeTmp[1] = estVergeL; 
		visNodeTmp[2] = estVergeR; 
		fixedMap_->getClosestInputNodeLink(&(visNodeTmp[0]), &(visNode[0]), &(tableNode[0]));
		
		estTabDist  = tableNode[0];
		estTabAngle = tableNode[1];
		
		
		// 3. estimate actual tilt, verge L and R on based on
		//  the actual table position 
		tableNodeTmp[0] = actTabDist; 
		tableNodeTmp[1] = actTabAngle; 
		             
		fixedMap_->getClosestOutputNodeLink(&(tableNodeTmp[0]),&(visNodeTmp[0]),&(tableNode[0]));
		currTiltDelta   =  visNodeTmp[0] - actTilt;
		currVergeLDelta =  visNodeTmp[1] - actVergeL;
		currVergeRDelta =  visNodeTmp[2] - actVergeR;
		
		adaptInpNode[0] = actTilt;
		adaptInpNode[1] = actVergeL;
		adaptInpNode[2] = actVergeR;
		adaptOutNode[0] = currTiltDelta;
		adaptOutNode[1] = currVergeLDelta;
		adaptOutNode[2] = currVergeRDelta;
		
		posError = distanceOnTable(estTabAngle, estTabDist,actTabAngle, actTabDist);
		
		// adaptation, deal with new nodes or replacement
		
		adaptingCalibMap_->incAgeForAll(1);
		if(fabs(posError) > fabs(noiseLevel_)){
			// bad  estimation
			if(adaptingCalibMap_->nmbOfLinks() > K_){
				// no additional can be link added		
				// replace the link with the bad estimation
				replaceBadEstimation(adaptingCalibMap_,adaptInpNode,adaptOutNode);
			}else{
				adaptingCalibMap_->addLink(adaptInpNode,adaptOutNode);
			};
		}else{
			// satisfying estimation
			// make the link with good estimation young again
			makeYoungGoodEstimation(adaptingCalibMap_,adaptInpNode,adaptOutNode);			
		};

		// remove very old nodes
		removeVeryOldLinks(adaptingCalibMap_,K_);
	


		cout <<  
			"\t" << adaptingCalibMap_->nmbOfLinks() << 
			"\t" << posError << 
			"\t" << adaptingCalibMap_->averageAge() <<
			"\t" << adaptingCalibMap_->getHighestAge() << 
			"\t" << noiseLevel_;
		// global mapping test
		float d, a;
		
		for(d = dMinT_; d<dMaxT_; d=d+dStep_){
			for(a = aMinT_; a<aMaxT_; a=a+aStep_){
				meanError = meanErrTableDist(adaptingCalibMap_,fixedMap_,eventsTests_,d,(d+dStep_),a,(a+aStep_));
				stdError  = stdDevTableDist(adaptingCalibMap_,fixedMap_,eventsTests_, meanError,d,(d+dStep_),a,(a+aStep_));
				cout << "\t" << meanError << "\t" << stdError;
			};
		};
		
		cout << "\n";

		
		
		
	}; //  while
	
	adaptingCalibMap_->writeXmlFile("visAdapt.xml");
	exit(0);
	
};




void removeVeryOldLinks(CAgingMapping *m, int capacity){
	int inpDim = m->getInpDim();
	int outDim = m->getOutDim();
	int atrDim = m->getAttrDim();

	// get index of the oldest node
	int size = m->nmbOfLinks();
	int oldestAge = -1000000;
	int oldestAgeIdx;
	float *ptrLink;
	for(int i = 0; i < size; i++){
		ptrLink = m->ptrLinkByIndex(i);
		if(ptrLink[inpDim + outDim] > oldestAge){
			oldestAge = ptrLink[inpDim + outDim];
			oldestAgeIdx = i;
		};
	};
		
	// test if age > capacity
	if(oldestAge > capacity){
		m->removeLink(oldestAgeIdx);
	};
}

void makeYoungGoodEstimation(CAgingMapping *m, float *inpNode, float *outNode){
	int inpDim = m->getInpDim();
	int outDim = m->getOutDim();
	int atrDim = m->getAttrDim();

	
	// find the node closest to inpNode
	int nmbLinks = m->nmbOfLinks();
	float *link;
	float *goodEstInpNode = new float(inpDim);
	float *goodEstOutNode = new float(outDim);
	
	m->getClosestInputNodeLink(inpNode,goodEstInpNode,goodEstOutNode);
	
	for(int i=0; i < nmbLinks; i++){
		link = m->ptrLinkByIndex(i);
		if((link[0] == goodEstInpNode[0]) && 
				(link[1] == goodEstInpNode[1]) && 
				(link[2] == goodEstInpNode[2]) &&
				(link[3] == goodEstOutNode[0]) &&
				(link[4] == goodEstOutNode[1]) &&
				(link[5] == goodEstOutNode[2])){
			// replace to link with closest input node 
			// because it has coused the bad estimation
			link = m->ptrLinkByIndex(i);
			*(&(link[inpDim + outDim])) = 0.0; // age
			
			break;
		};
	};
	
	
	
	delete [] goodEstInpNode;
	delete [] goodEstOutNode;
	return;
}

void replaceBadEstimation(CAgingMapping *m, float *inpNode, float *outNode){
	int inpDim = m->getInpDim();
	int outDim = m->getOutDim();
	int atrDim = m->getAttrDim();


	// find the node closest to inpNode
	int nmbLinks = m->nmbOfLinks();
	float *link;
	float *badEstInpNode = new float(inpDim);
	float *badEstOutNode = new float(outDim);
	
	m->getClosestInputNodeLink(inpNode,badEstInpNode,badEstOutNode);
	
	for(int i=0; i < nmbLinks; i++){
		link = m->ptrLinkByIndex(i);
		if((link[0] == badEstInpNode[0]) && 
				(link[1] == badEstInpNode[1]) && 
				(link[2] == badEstInpNode[2]) &&
				(link[3] == badEstOutNode[0]) &&
				(link[4] == badEstOutNode[1]) &&
				(link[5] == badEstOutNode[2])){
			// replace to link with closest input node 
			// because it has coused the bad estimation
			link = m->ptrLinkByIndex(i);
			for(int i = 0; i < inpDim; i++){
				*(&(link[i])) = inpNode[i];
			};
			for(int i = 0; i < outDim; i++){
				*(&(link[i])) = inpNode[inpDim + i];
			};
			*(&(link[inpDim + outDim])) = 0.0; // age
			
			break;
		};
	};
	
	
	
	delete [] badEstInpNode;
	delete [] badEstOutNode;
	return;
};


void replaceRandomly(CAgingMapping *m, float *inpNode, float *outNode){
	int inpDim = m->getInpDim();
	int outDim = m->getOutDim();
	int atrDim = m->getAttrDim();
	int nmbLinks = m->nmbOfLinks();
	
	int randIdx = (int)(((float)nmbLinks)*
			(((float)random()) / ((float) RAND_MAX)));
	float *link = m->ptrLinkByIndex(randIdx);
	for(int i = 0; i < inpDim; i++){
		*(&(link[i])) = inpNode[i];
	};
	for(int i = 0; i < outDim; i++){
		*(&(link[i])) = inpNode[inpDim + i];
	};
	for(int i = 0; i < atrDim; i++){
		*(&(link[i])) = inpNode[inpDim + outDim +  i];
	};
	return;
};



int info(int argc, char* args[]){
	cout << "\n usage of " << args[0] << " : \n\n";
	cout << args[0] << 
	"\t mapFixed.xml \t mapDelta.xml " <<
	"\t mapEvents.xml \t mapEventsTests.xml \n\n";		
};

float meanErrTableDist(CAgingMapping *calibMap,CXmlMap *fixedMap, CXmlMap *testSamples, float dMin, float dMax, float aMin, float aMax){
	float sum = 0.0;
	int samples = testSamples->nmbOfLinks();
	float *link;
	float actTabDist,  actTabAngle, actTilt, actVergeL, actVergeR;
	float estTabAngle, estTabDist,  estTilt, estVergeL, estVergeR;
	float estTiltDelta, estVergeLDelta, estVergeRDelta;
	float currTiltDelta, currVergeLDelta, currVergeRDelta;
	float posError, meanError, stdError;
	float *tableNode    = new float(2);
	float *tableNodeTmp = new float(2);
	float *visNode      = new float(3);
	float *visNodeTmp   = new float(3); 
	float *adaptInpNode = new float(3);
	float *adaptOutNode = new float(3);

	
	
	int validSamples = 0;
	int i = 0;
	while(i < samples){
		link = testSamples->ptrLinkByIndex(i);	
		actTilt      = link[0];
		actVergeL    = link[1];
		actVergeR    = link[2];
		actTabDist   = link[3];
		actTabAngle  = link[4];
		i++;

		// test limits
		if((actTabDist < dMin) || (actTabDist > dMax)){
			continue;
		};
		if((actTabAngle < aMin) || (actTabAngle > aMax)){
			continue;
		};
		validSamples++;

		
		// 1. estimate shift of visual space
		adaptingCalibMap_->getClosestInputNodeLink(&(link[0]),&(adaptInpNode[0]),&(adaptOutNode[0]));
		estTiltDelta   = adaptOutNode[0];
		estVergeLDelta = adaptOutNode[1];
		estVergeRDelta = adaptOutNode[2];
		
		
		// estimation via calculation
		// quadratic
		//estTiltDelta   = qf(actTilt,-1.408,-1.1651,-0.2385); 
		//estVergeLDelta = qf(actVergeL, 0.527,-0.208,-0.246);
		//estVergeRDelta = qf(actVergeR, 0.4776,-0.0838,-0.268); 
		//linear 
		//estTiltDelta   = qf(actTilt,0.0  ,-0.0529,-0.0212); 
		//estVergeLDelta = qf(actVergeL,0.0, 0.073 ,-0.2569);
		//estVergeRDelta = qf(actVergeR,0.0,-0.0187,-0.243);
		//avg 
		//estTiltDelta   = qf(actTilt,  0.0,0.0,-0.0006); 
		//estVergeLDelta = qf(actVergeL,0.0,0.0,-0.2359);
		//estVergeRDelta = qf(actVergeR,0.0,0.0,-0.2442);
		//none 
		//estTiltDelta   = qf(actTilt,  0.0,0.0,0.0); 
		//estVergeLDelta = qf(actVergeL,0.0,0.0,0.0);
		//estVergeRDelta = qf(actVergeR,0.0,0.0,0.0);

		
		
		estTilt   = actTilt + estTiltDelta;
		estVergeL = actVergeL + estVergeLDelta;
		estVergeR = actVergeR + estVergeRDelta;
		
		// 2. estimate table position angle and distance via fixed map
		visNodeTmp[0] = estTilt; 
		visNodeTmp[1] = estVergeL; 
		visNodeTmp[2] = estVergeR; 
		fixedMap_->getClosestInputNodeLink(&(visNodeTmp[0]), &(visNode[0]), &(tableNode[0]));
		
		estTabDist  = tableNode[0];
		estTabAngle = tableNode[1];

		
		sum = sum + distanceOnTable(actTabAngle,actTabDist,estTabAngle,estTabDist);	
	};
	
	if(i < 1) return (-1.0);
	return (sum / ((float) validSamples));
};


float stdDevTableDist(CAgingMapping *calibMap,CXmlMap *fixedMap, CXmlMap *testSamples, float meanErr, float dMin, float dMax, float aMin, float aMax){
	float sum = 0.0;
	int samples = testSamples->nmbOfLinks();
	float *link;
	float actTabDist,  actTabAngle, actTilt, actVergeL, actVergeR;
	float estTabAngle, estTabDist,  estTilt, estVergeL, estVergeR;
	float estTiltDelta, estVergeLDelta, estVergeRDelta;
	float currTiltDelta, currVergeLDelta, currVergeRDelta;
	float posError, meanError, stdError;
	float *tableNode    = new float(2);
	float *tableNodeTmp = new float(2);
	float *visNode      = new float(3);
	float *visNodeTmp   = new float(3); 
	float *adaptInpNode = new float(3);
	float *adaptOutNode = new float(3);

	float error;
	
	int validSamples = 0;
	int i = 0;
	while(i < samples){
		link = testSamples->ptrLinkByIndex(i);	
		actTilt      = link[0];
		actVergeL    = link[1];
		actVergeR    = link[2];
		actTabDist   = link[3];
		actTabAngle  = link[4];
		i++;

		// test limits
		if((actTabDist < dMin) || (actTabDist > dMax)){
			continue;
		};
		if((actTabAngle < aMin) || (actTabAngle > aMax)){
			continue;
		};
		validSamples++;

		
		// 1. estimate shift of visual space
		adaptingCalibMap_->getClosestInputNodeLink(&(link[0]),&(adaptInpNode[0]),&(adaptOutNode[0]));
		estTiltDelta   = adaptOutNode[0];
		estVergeLDelta = adaptOutNode[1];
		estVergeRDelta = adaptOutNode[2];
		
		// estimation via calculation
		// quadratic
		//estTiltDelta   = qf(actTilt,-1.408,-1.1651,-0.2385); 
		//estVergeLDelta = qf(actVergeL, 0.527,-0.208,-0.246);
		//estVergeRDelta = qf(actVergeR, 0.4776,-0.0838,-0.268); 
		//linear 
		//estTiltDelta   = qf(actTilt,0.0  ,-0.0529,-0.0212); 
		//estVergeLDelta = qf(actVergeL,0.0, 0.073 ,-0.2569);
		//estVergeRDelta = qf(actVergeR,0.0,-0.0187,-0.243);
		//avg 
		//estTiltDelta   = qf(actTilt,  0.0,0.0,-0.0006); 
		//estVergeLDelta = qf(actVergeL,0.0,0.0,-0.2359);
		//estVergeRDelta = qf(actVergeR,0.0,0.0,-0.2442);
		//none 
		//estTiltDelta   = qf(actTilt,  0.0,0.0,0.0); 
		//estVergeLDelta = qf(actVergeL,0.0,0.0,0.0);
		//estVergeRDelta = qf(actVergeR,0.0,0.0,0.0);

		
		
		estTilt   = actTilt + estTiltDelta;
		estVergeL = actVergeL + estVergeLDelta;
		estVergeR = actVergeR + estVergeRDelta;


		
		
		// 2. estimate table position angle and distance via fixed map
		visNodeTmp[0] = estTilt; 
		visNodeTmp[1] = estVergeL; 
		visNodeTmp[2] = estVergeR; 
		fixedMap_->getClosestInputNodeLink(&(visNodeTmp[0]), &(visNode[0]), &(tableNode[0]));
		
		estTabDist  = tableNode[0];
		estTabAngle = tableNode[1];

		error = distanceOnTable(actTabAngle,actTabDist,estTabAngle,estTabDist);
		sum = sum + ((error - meanErr)*(error - meanErr));	
	};
	if(i < 1) return (-1.0);
	return (sqrt(sum / ((float) validSamples)) );
};

float distanceOnTable(float angleRad1, float dist1, float angleRad2, float dist2){
	double x1 = dist1*sin(angleRad1); // given in rad
	double y1 = dist1*cos(angleRad1); // given in rad
	double x2 = dist2*sin(angleRad2); // given in rad
	double y2 = dist2*cos(angleRad2); // given in rad
	
	double d = (x1 - x2) * (x1 - x2); 
	d = d + ((y1 - y2) * (y1 - y2));
	d = sqrt(fabs(d));
	
	//cout << "dis  1 : " << dist1 << endl;
	//cout << "angl 1 : " << angleRad1 << endl;	
	//cout << "dis  2 : " << dist2 << endl;
	//cout << "angl 2 : " << angleRad2 << endl;
	//cout << "===> d : " << d << endl << endl; 

	
	return ((float)d);
};

float qf(float x, float a,float b, float c){
	return ((a*x*x) + (b*x) + c);
};

