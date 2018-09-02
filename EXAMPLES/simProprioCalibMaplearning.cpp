#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include <iostream>
#include <stdexcept>
#include <math.h>
#include <float.h>

#include "CXmlMap.h"


XERCES_CPP_NAMESPACE_USE

	
using namespace std;

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
CXmlMap *events1_;
CXmlMap *events1Tests_;
CXmlMap *events2_;
CXmlMap *events2Tests_;
CXmlMap *fixedMap_;
CAgingMapping *adaptingCalibMap_;

// variables

bool shifted_ = false;

float noiseLevel_ = 0.0;
float K_ = 500;

int steps_ = 300;

// domain limits
float aMin_ = -2.4;
float aMax_ = 2.4; 
float dMin_ = 0;
float dMax_ = 100;





// functions
int info(int argc, char* args[]);
float meanErrTableDist(CAgingMapping *mapping,CXmlMap *fixedMap,CXmlMap *testSamples, float dMin, float dMax, float aMin, float aMax);
float stdDevTableDist(CAgingMapping *mapping,CXmlMap *fixedMap, CXmlMap *testSamples, float meanErr, float dMin, float dMax, float aMin, float aMax);
float distanceOnTable(float angle1, float dist1, float angle2, float dist2);
void replaceBadEstimation(CAgingMapping *m, float *inpNode, float *outNode);
void makeYoungGoodEstimation(CAgingMapping *m, float *inpNode, float *outNode);
void removeVeryOldLinks(CAgingMapping *m, int capacity);

int main (int argc, char* args[]) {
	if(argc != 6){info(argc,args); return (0);};
	
	try{
		events1_          = new CXmlMap();
		events1Tests_     = new CXmlMap();
		events2_          = new CXmlMap();
		events2Tests_     = new CXmlMap();
		fixedMap_         = new CXmlMap();
		adaptingCalibMap_ = new CAgingMapping();

		fixedMap_    ->readXmlFileWithoutAttributes(args[1]);
		events1_     ->readXmlFileWithoutAttributes(args[2]);
		events1Tests_->readXmlFileWithoutAttributes(args[3]);
		events2_     ->readXmlFileWithoutAttributes(args[4]);
		events2Tests_->readXmlFileWithoutAttributes(args[5]);
	}catch(...){
		cout << "Can't read mappings from files, terminate!\n\n";
		exit(0);
	};
	
	CXmlMap *eventPtr;
	CXmlMap *eventTestPtr;
	float   *link;
	
	float actTabDist, actTabAngle, actTilt, actVergeL, actVergeR;
	float estTabDist, estTabAngle, estTabDistDelta, estTabAngleDelta;
	float currTabDistDelta, currTabAngleDelta;
	float posError, meanError, stdError;
	float *tableNode    = new float(2);
	float *tableNodeTmp = new float(2);
	float *visNode      = new float(3);
	float *visNodeTmp   = new float(3); 
	float *adaptInpNode = new float(2);
	float *adaptOutNode = new float(2);
	
	adaptingCalibMap_->init(2,2,1,K_);
	adaptInpNode[0] = 0.0;
	adaptInpNode[1] = 0.0;
	adaptingCalibMap_->addLink(adaptInpNode,adaptInpNode);
	
	int eventIdx, event1Idx, event2Idx;
	event1Idx = 0;
	event2Idx = 0;
	int i = 0;
	while(i < steps_){
		
		// get data actual data tilt, verge L and R and 
		// table distance and angle
		if(!shifted_){
			eventPtr = events1_;
			eventTestPtr = events1Tests_;
			if(event1Idx < eventPtr->nmbOfLinks()){
				event1Idx++;
			}else{
				event1Idx = 0;
			};
			eventIdx = event1Idx;
			event1Idx++;
		}else{
			eventPtr = events2_;
			eventTestPtr = events2Tests_;			
			if(event2Idx < eventPtr->nmbOfLinks()){
				event2Idx++;
			}else{
				event2Idx = 0;
			};
			eventIdx = event2Idx;
			event2Idx++;
		};
		link = eventPtr->ptrLinkByIndex(eventIdx);
		i++;
		if(link == NULL) continue;
		
		actTilt      = link[0];
		actVergeL    = link[1];
		actVergeR    = link[2];
		actTabDist   = link[3];
		actTabAngle  = link[4];
		
		
		
		// 1. estimate table position angle and distance via fixed map
		fixedMap_->getClosestInputNodeLink(&(link[0]), &(visNodeTmp[0]), &(tableNodeTmp[0]));
		// 2. estimate shift of angle and distance via adapting map
		adaptingCalibMap_->getClosestInputNodeLink(&(tableNodeTmp[0]),&(adaptInpNode[0]),&(adaptOutNode[0]));
		
		estTabDistDelta  = adaptOutNode[0];
		estTabAngleDelta = adaptOutNode[1];
		estTabDist       = tableNodeTmp[0] + estTabDistDelta;
		estTabAngle      = tableNodeTmp[1] + estTabAngleDelta;
		
		
		
		currTabDistDelta  = estTabAngle - actTabAngle;
		currTabAngleDelta = estTabDist  - actTabDist;
		adaptOutNode[0]   = currTabDistDelta;
		adaptOutNode[1]   = currTabAngleDelta;
		adaptInpNode[0]   = actTabDist;
		adaptInpNode[1]   = actTabAngle;
		
		
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
	


		
		// global mapping test
		meanError = meanErrTableDist(adaptingCalibMap_,fixedMap_,eventTestPtr,dMin_,dMax_,aMin_,aMax_);
		stdError  = stdDevTableDist(adaptingCalibMap_,fixedMap_,eventTestPtr, meanError,dMin_,dMax_,aMin_,aMax_);

		
		cout << meanError << "\t" << stdError << 
			"\t" << adaptingCalibMap_->nmbOfLinks() << 
			"\t" << posError << 
			"\t" << adaptingCalibMap_->averageAge() <<
			"\t" << adaptingCalibMap_->getHighestAge() << 
			"\t" << noiseLevel_ << endl;
		
		
	}; //  while
	
	adaptingCalibMap_->writeXmlFile("proAdapt.xml");
	exit(0);
};




int info(int argc, char* args[]){
	cout << "\n usage of " << args[0] << " : \n\n";
	cout << args[0] << 
		"\t mapFixed.xml \t mapEvents1.xml \t mapEvents1Tests.xml " <<
		"\t mapEvents2.xml \t mapEvents2Tests.xml \n\n";		
	
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


float meanErrTableDist(CAgingMapping *mapping,CXmlMap *fixedMap,CXmlMap *testSamples, float dMin, float dMax, float aMin, float aMax){
	float sum = 0.0;
	int samples = testSamples->nmbOfLinks();
	float *link;

	float actTabDist, actTabAngle;
	float estTabDist, estTabAngle, estTabDistDelta, estTabAngleDelta;
	float posError;
	float *tableNode    = new float(2);
	float *visNode      = new float(3);
	float *adaptInpNode = new float(2);
	float *adaptOutNode = new float(2);

	int i = 0;
	int validSamples = 0;
	while(i < samples){
		link = testSamples->ptrLinkByIndex(i);
		i++;
		if(link == NULL) continue;
		
		actTabDist   = link[3];
		actTabAngle  = link[4];		
		// test limits
		if((actTabDist < dMin) || (actTabDist > dMax)){
			continue;
		};
		if((actTabAngle < aMin) || (actTabAngle > aMax)){
			continue;
		};
		validSamples++;
				
		
		// 1. estimate table position angle and distance via fixed map
		fixedMap->getClosestInputNodeLink(&(link[0]), visNode, tableNode);
		// 2. estimate shift of angle and distance via adapting map
		mapping->getClosestInputNodeLink(tableNode,adaptInpNode,adaptOutNode);
		
		estTabDistDelta  = adaptOutNode[0];
		estTabAngleDelta = adaptOutNode[1];
		estTabDist       = tableNode[0] + estTabDistDelta;
		estTabAngle      = tableNode[1] + estTabAngleDelta;
		
		
		posError = distanceOnTable(estTabAngle, estTabDist,actTabAngle, actTabDist);
		sum = sum + posError;	
	};
	
	if(i < 1) return (-1.0);
	return (sum / ((float) validSamples));
};

float stdDevTableDist(CAgingMapping *mapping,CXmlMap *fixedMap, CXmlMap *testSamples, float meanErr, float dMin, float dMax, float aMin, float aMax){
	float sum = 0.0;
	int samples = testSamples->nmbOfLinks();
	float *link;

	float actTabDist, actTabAngle;
	float estTabDist, estTabAngle, estTabDistDelta, estTabAngleDelta;
	float posError;
	float *tableNode    = new float(2);
	float *visNode      = new float(3);
	float *adaptInpNode = new float(2);
	float *adaptOutNode = new float(2);

	int i = 0;
	int validSamples = 0;
	while(i < samples){
		link = testSamples->ptrLinkByIndex(i);
		i++;
		if(link == NULL) continue;
		
		actTabDist   = link[3];
		actTabAngle  = link[4];		
		// test limits
		if((actTabDist < dMin) || (actTabDist > dMax)){
			continue;
		};
		if((actTabAngle < aMin) || (actTabAngle > aMax)){
			continue;
		};
		validSamples++;
				
		
		// 1. estimate table position angle and distance via fixed map
		fixedMap->getClosestInputNodeLink(&(link[0]), visNode, tableNode);
		// 2. estimate shift of angle and distance via adapting map
		mapping->getClosestInputNodeLink(tableNode,adaptInpNode,adaptOutNode);
		
		estTabDistDelta  = adaptOutNode[0];
		estTabAngleDelta = adaptOutNode[1];
		estTabDist       = tableNode[0] + estTabDistDelta;
		estTabAngle      = tableNode[1] + estTabAngleDelta;
		
		
		posError = distanceOnTable(estTabAngle, estTabDist,actTabAngle, actTabDist);
		sum = sum + ( (posError - meanErr)*(posError - meanErr));	
	};
	
	if(i < 1) return (-1.0);
	return (sqrt(sum / ((float) validSamples)));
};

void replaceBadEstimation(CAgingMapping *m, float *inpNode, float *outNode){
	// find the node closest to inpNode
	int nmbLinks = m->nmbOfLinks();
	float *link;
	float *badEstInpNode = new float(2);
	float *badEstOutNode = new float(2);
	
	m->getClosestInputNodeLink(inpNode,badEstInpNode,badEstOutNode);
	
	for(int i=0; i < nmbLinks; i++){
		link = m->ptrLinkByIndex(i);
		if((link[0] == badEstInpNode[0]) && 
				(link[1] == badEstInpNode[1]) && 
				(link[3] == badEstOutNode[0]) &&
				(link[4] == badEstOutNode[1])){
			// replace to link with closest input node 
			// because it has coused the bad estimation
			link = m->ptrLinkByIndex(i);
			*(&(link[0])) = inpNode[0];
			*(&(link[1])) = inpNode[1];
			*(&(link[2])) = outNode[0];
			*(&(link[3])) = outNode[1];
			*(&(link[4])) = 0.0; // age
			
			break;
		};
	};
	
	
	
	delete [] badEstInpNode;
	delete [] badEstOutNode;
	return;
};

void makeYoungGoodEstimation(CAgingMapping *m, float *inpNode, float *outNode){
	// find the node closest to inpNode
	int nmbLinks = m->nmbOfLinks();
	float *link;
	float *goodEstInpNode = new float(2);
	float *goodEstOutNode = new float(2);
	
	m->getClosestInputNodeLink(inpNode,goodEstInpNode,goodEstOutNode);
	
	for(int i=0; i < nmbLinks; i++){
		link = m->ptrLinkByIndex(i);
		if((link[0] == goodEstInpNode[0]) && 
				(link[1] == goodEstInpNode[1]) && 
				(link[2] == goodEstOutNode[0]) &&
				(link[3] == goodEstOutNode[1])){
			// replace to link with closest input node 
			// because it has coused the bad estimation
			link = m->ptrLinkByIndex(i);
			*(&(link[4])) = 0.0; // age
			
			break;
		};
	};
	
	delete [] goodEstInpNode;
	delete [] goodEstOutNode;
	return;
};


void removeVeryOldLinks(CAgingMapping *m, int capacity){
	// get index of the oldest node
	int size = m->nmbOfLinks();
	int oldestAge = -1000000;
	int oldestAgeIdx;
	float *ptrLink;
	for(int i = 0; i < size; i++){
		ptrLink = m->ptrLinkByIndex(i);
		if(ptrLink[4] > oldestAge){
			oldestAge = ptrLink[4];
			oldestAgeIdx = i;
		};
	};
		
	// test if age > capacity
	if(oldestAge > capacity){
		m->removeLink(oldestAgeIdx);
	};
};


