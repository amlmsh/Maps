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
			*(&(ptrLink[5])) += add;
		};
	};
	void setAgeForAll(int age){
		if(age < 0) age = 0;
		// assume 1st component of the attribute values
		// is the age and (inpDim + outDim) = 5
		float *ptrLink;
		for(int i = 0; i < nmbOfLinks();i++){
			ptrLink = ptrLinkByIndex(i);
			*(&(ptrLink[5])) = age;
		};
	};
	
	int getHighestAge(){
		int oldestAge = 0;
		float *ptrLink;
		for(int i = 0; i < nmbOfLinks();i++){
			ptrLink = ptrLinkByIndex(i);
			if(ptrLink[5] > oldestAge){
				oldestAge = ptrLink[5];
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
			age = age + ptrLink[5];
		};
		return (((float) age) / ((float) size));
	};
	
};


// constants and global stuff
CXmlMap *shiftedViewEvents_;
CXmlMap *shiftedViewTests_;
CXmlMap *centredViewEvents_;
CXmlMap *centredViewTests_;

CAgingMapping *adaptingMap_;

float *inpNode_;
float *outNode_;

bool shiftedView_ = false;


int completeTrails_;
float noiseLevel_ = 0.0;
int K_ = 300;

int offset = 0;

int endTime_ = 300 + offset;
int shift1st = offset;
int shift1stBack = 4500;
int shift2nd = 3000;
int shift2ndBack = 5000;


// functions
int info(int argc, char* args[]);
float meanErrTableDist(CAgingMapping *mapping, CXmlMap *testSamples);
float stdDevTableDist(CAgingMapping *mapping, CXmlMap *testSamples, float meanErr);
float distanceOnTable(float angle1, float dist1, float angle2, float dist2);
void replaceBadEstimation(CAgingMapping *m, float *inpNode, float *outNode);
void makeYoungGoodEstimation(CAgingMapping *m, float *inpNode, float *outNode);
void replaceRandomly(CAgingMapping *m, float *inpNode, float *outNode);
void removeVeryOldLinks(CAgingMapping *m, int capacity);

int main (int argc, char* args[]) {
	if(argc != 5){info(argc,args); return (0);};

	// define mapping function or the real world
	srand(time(NULL));

	shiftedViewEvents_ = new CXmlMap();
	shiftedViewTests_  = new CXmlMap();
	centredViewEvents_ = new CXmlMap();
	centredViewTests_  = new CXmlMap();
	adaptingMap_       = new CAgingMapping();
	
	try{
		centredViewEvents_->readXmlFileWithoutAttributes(args[1]);
		centredViewTests_->readXmlFileWithoutAttributes(args[2]);
		shiftedViewEvents_->readXmlFileWithoutAttributes(args[3]);
		shiftedViewTests_->readXmlFileWithoutAttributes(args[4]);
	}catch(...){
		cout << "Couldn't read xml files. Terminate.\n\n";
		return (0);
	};
	
	inpNode_ = new float(3);
	outNode_ = new float(2);
	float *inpNodeTmp = new float(3);
	float *outNodeTmp = new float(2);
	
	int nmbCentredEvents = centredViewEvents_->nmbOfLinks();
	int nmbShiftedEvents = shiftedViewEvents_->nmbOfLinks();
	completeTrails_ = nmbCentredEvents + nmbShiftedEvents;
	adaptingMap_->init(3,2,1,completeTrails_);
	
	
	// define mapping function or the real world
	srand(time(NULL));
	
	// init mapping M (empty or learned)

	
	int i=0;
	int indexShiftedView_ = 0;
	int indexCentredView_ = 0;
	float actualArmAngle;
	float actualArmDistance;
	float estimatedArmAngle;
	float estimatedArmDistance;
	
	float *refNode;
	
	float positionError;
	float meanError;
	float stdError;
	
	do{

		// do you want to change the world?
		if((i == shift1st) ||(i == shift2nd)){
			shiftedView_ = true;
		};
		if((i == shift1stBack) ||(i == shift2ndBack)){
			shiftedView_ = false;
		};
		
		
		// get next table position and resulting active vision values,
		// assume that this position
		// was selected randomly via the gap-method when recorded
		if(shiftedView_){
			if(indexShiftedView_ >= nmbShiftedEvents){
				indexShiftedView_ = 0;
			};
			refNode = shiftedViewEvents_->ptrLinkByIndex(indexShiftedView_);
			indexShiftedView_++;
		}else{
			if(indexCentredView_ >= nmbCentredEvents){
				indexCentredView_ = 0;
			};
			refNode = centredViewEvents_->ptrLinkByIndex(indexCentredView_);
			indexCentredView_++;
		};
		inpNode_[0] = refNode[0];
		inpNode_[1] = refNode[1];
		inpNode_[2] = refNode[2];
		outNode_[0] = refNode[3];
		outNode_[1] = refNode[4];
		                         
		
	
		// do the estimation of table position
		//    based on the adapting mapping
		adaptingMap_->getClosestInputNodeLink(inpNode_,inpNodeTmp,outNodeTmp);
		estimatedArmDistance = outNodeTmp[0];
		estimatedArmAngle    = outNodeTmp[1];
		actualArmDistance = outNode_[0];
		actualArmAngle    = outNode_[1];

		// calculate error (table distance)
		positionError = 
			distanceOnTable(estimatedArmAngle,estimatedArmDistance, actualArmAngle,actualArmDistance);

		
		// deal with new nodes or replacement
		if(fabs(positionError) > fabs(noiseLevel_)){
			// bad  estimation
			//replaceBadEstimation(adaptingMap_,inpNode_,outNode_);
			if(adaptingMap_->nmbOfLinks() > K_){
				// no additional can be link added		
				// replace the link with the bad estimation
				//replaceBadEstimation(adaptingMap_,inpNode_,outNode_);
				adaptingMap_->incAgeForAll(1);
			}else{
				adaptingMap_->incAgeForAll(1);
				adaptingMap_->addLink(inpNode_,outNode_);
			};
			

		}else{
			// satisfying estimation
			// make the link with good estimation young again
			adaptingMap_->incAgeForAll(1);
			makeYoungGoodEstimation(adaptingMap_,inpNode_,outNode_);
			
		};

		// remove very old nodes
		//removeVeryOldLinks(adaptingMap_,K_);
		//removeVeryOldLinks(adaptingMap_,adaptingMap_->nmbOfLinks());

		
		// test the whole current mapping
		if(shiftedView_){
			meanError = meanErrTableDist(adaptingMap_,shiftedViewTests_);
			stdError  = stdDevTableDist(adaptingMap_,shiftedViewTests_, meanError);
		}else{
			meanError = meanErrTableDist(adaptingMap_,centredViewTests_);
			stdError  = stdDevTableDist(adaptingMap_,centredViewTests_, meanError);
		};
		

		//cout << positionError << endl;
		/**
		cout << meanError << "\t" << stdError << 
			"\t" << ((float)adaptingMap_->nmbOfLinks() / (float) K_)*10 << 
			"\t" << positionError << 
			"\t" << ((float)adaptingMap_->getHighestAge() / (float) K_)*10 << endl;
			*/
		
		cout << meanError << "\t" << stdError << 
			"\t" << adaptingMap_->nmbOfLinks() << 
			"\t" << positionError << 
			"\t" << adaptingMap_->averageAge() <<
			"\t" << adaptingMap_->getHighestAge() << 
			"\t" << noiseLevel_ << endl;
		 
		
		i++;
	}while((i < completeTrails_) && (i < endTime_ ));
	
	adaptingMap_->writeXmlFile("adaptMap.xml");
	
};




void removeVeryOldLinks(CAgingMapping *m, int capacity){
	// get index of the oldest node
	int size = m->nmbOfLinks();
	int oldestAge = -1000000;
	int oldestAgeIdx;
	float *ptrLink;
	for(int i = 0; i < size; i++){
		ptrLink = m->ptrLinkByIndex(i);
		if(ptrLink[5] > oldestAge){
			oldestAge = ptrLink[5];
			oldestAgeIdx = i;
		};
	};
		
	// test if age > capacity
	if(oldestAge > capacity){
		m->removeLink(oldestAgeIdx);
	};
}

void makeYoungGoodEstimation(CAgingMapping *m, float *inpNode, float *outNode){
	// find the node closest to inpNode
	int nmbLinks = m->nmbOfLinks();
	float *link;
	float *goodEstInpNode = new float(3);
	float *goodEstOutNode = new float(2);
	
	m->getClosestInputNodeLink(inpNode,goodEstInpNode,goodEstOutNode);
	
	for(int i=0; i < nmbLinks; i++){
		link = m->ptrLinkByIndex(i);
		if((link[0] == goodEstInpNode[0]) && 
				(link[1] == goodEstInpNode[1]) && 
				(link[2] == goodEstInpNode[2]) &&
				(link[3] == goodEstOutNode[0]) &&
				(link[4] == goodEstOutNode[1])){
			// replace to link with closest input node 
			// because it has coused the bad estimation
			link = m->ptrLinkByIndex(i);
			*(&(link[5])) = 0.0; // age
			
			break;
		};
	};
	
	
	
	delete [] goodEstInpNode;
	delete [] goodEstOutNode;
	return;
}

void replaceBadEstimation(CAgingMapping *m, float *inpNode, float *outNode){
	// find the node closest to inpNode
	int nmbLinks = m->nmbOfLinks();
	float *link;
	float *badEstInpNode = new float(3);
	float *badEstOutNode = new float(2);
	
	m->getClosestInputNodeLink(inpNode,badEstInpNode,badEstOutNode);
	
	for(int i=0; i < nmbLinks; i++){
		link = m->ptrLinkByIndex(i);
		if((link[0] == badEstInpNode[0]) && 
				(link[1] == badEstInpNode[1]) && 
				(link[2] == badEstInpNode[2]) &&
				(link[3] == badEstOutNode[0]) &&
				(link[4] == badEstOutNode[1])){
			// replace to link with closest input node 
			// because it has coused the bad estimation
			link = m->ptrLinkByIndex(i);
			*(&(link[0])) = inpNode[0];
			*(&(link[1])) = inpNode[1];
			*(&(link[2])) = inpNode[2];
			*(&(link[3])) = outNode[0];
			*(&(link[4])) = outNode[1];
			*(&(link[5])) = 0.0; // age
			
			break;
		};
	};
	
	
	
	delete [] badEstInpNode;
	delete [] badEstOutNode;
	return;
};


void replaceRandomly(CAgingMapping *m, float *inpNode, float *outNode){
	int nmbLinks = m->nmbOfLinks();
	int randIdx = (int)(((float)nmbLinks)*
			(((float)random()) / ((float) RAND_MAX)));
	float *link = m->ptrLinkByIndex(randIdx);
	*(&(link[0])) = inpNode[0];
	*(&(link[1])) = inpNode[1];
	*(&(link[2])) = inpNode[2];
	*(&(link[3])) = outNode[0];
	*(&(link[4])) = outNode[1];
	return;
};



int info(int argc, char* args[]){
	cout << "\n usage of " << args[0] << " : \n\n";
	cout << args[0] << 
		"mapCentredViewEvents.xml  mapCentredViewTests.xml  " <<
		"mapShiftedViewEvents.xml  mapShiftedViewTests.xml \n\n";		
	
};

float meanErrTableDist(CAgingMapping *mapping, CXmlMap *testSamples){
	float sum = 0.0;
	int samples = testSamples->nmbOfLinks();
	float *node;
	float estTabDist, actTabDist;
	float estTabAngle, actTabAngle;
	float activeVisionCoord[3];
	
	float inpNode[3];
	float outNode[2];
	
	int i = 0;
	do{
		node = testSamples->ptrLinkByIndex(i);
		actTabDist = node[3];
		actTabAngle = node[4];
		
		// do estimation
		mapping->getClosestInputNodeLink(node,&(inpNode[0]),&(outNode[0]));
		estTabDist = outNode[0];
		estTabAngle = outNode[1];
		
		sum = sum + distanceOnTable(actTabAngle,actTabDist,estTabAngle,estTabDist);	
		i++;
	}while(i < samples);
	
	
	return (sum / ((float) samples));
}
float stdDevTableDist(CAgingMapping *mapping, CXmlMap *testSamples, float meanErr){
	int samples = testSamples->nmbOfLinks();
	float *node;
	float estTabDist, actTabDist;
	float estTabAngle, actTabAngle;
	float activeVisionCoord[3];
	
	float inpNode[3];
	float outNode[2];
	float error;
	float sum = 0.0;
	
	int i = 0;
	do{
		node = testSamples->ptrLinkByIndex(i);
		actTabDist = node[3];
		actTabAngle = node[4];
		
		// do estimation
		mapping->getClosestInputNodeLink(node,&(inpNode[0]),&(outNode[0]));
		estTabDist = outNode[0];
		estTabAngle = outNode[1];
		error = distanceOnTable(actTabAngle,actTabDist,estTabAngle,estTabDist);
		sum += ((error - meanErr)*(error - meanErr));	
		i++;
	}while(i < samples);
	
	
	return (sqrt(sum / ((float) samples)) );
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
