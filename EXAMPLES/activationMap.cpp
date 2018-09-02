#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include <iostream>
#include <stdexcept>
#include <math.h>
//opencv 
#include <opencv2/opencv.hpp>


#include "CXmlMap.h"
#include "CMemoryMap.h"

using namespace std;

XERCES_CPP_NAMESPACE_USE


// views
IplImage *openCvInitImage_;
IplImage *openCvSystemImage_;
char *initWinName_;
char *systemWinName_;
int WINDOW_WIDTH_;
int WINDOW_HEIGHT_;

// domain
int range_;
bool zeroCentred_;
bool accumulation_;
int inpDim_;

void createRandomConnections(CXmlMap *map, int range, int nmbConnections);
void createCentredConnections(CXmlMap *map, int range, int nmbConnections);

void initWindows(char *initWinName, char *systemWinName);
void deinitWindows();

void refreshInitWin(float *nodes, int nmbNodes);
void refreshSystemWin(float *nodes, int nmbNodes);
void refreshWindow(IplImage *openCvImage, char *windowName, float *nodes, int nmbNodes, bool accumulation, int accValue);


float *getPattern(int nmbNodes, int range, bool zeroCentr);

int main (int argc, char* args[]) {
	CXmlMap map;
	CMemoryMap mem;
	inpDim_ = 2;
	int outDim = inpDim_;
	range_  =  75;
	int nmbInitNodes = 1000;
	zeroCentred_ = true;
	float probConnection =  0.000025;
	int nmbLinks = (int)((float)range_*range_*range_*range_*probConnection);
	srand ( time(NULL) );
	accumulation_ = false;
	
	
	cout << "nmb. links: " << nmbLinks << endl;
	
	//exit(0);
	
	//init windows
	WINDOW_HEIGHT_ = range_;
	WINDOW_WIDTH_  = range_;
	initWindows("init","system state");
	
	
	// create  a map
	map.init(inpDim_,outDim,1,nmbLinks*2);
	if(zeroCentred_){
		createCentredConnections(&map, range_, nmbLinks);
	}else{
		createRandomConnections(&map, range_, nmbLinks);
	}

	//map.writeXmlFile("centr.xml");

	
	// init memory
	mem.initMap(&map);
	float *nodes;
	int    nmbNodes;
	unsigned int *actVec = new unsigned int[nmbLinks];

	
	
	// init activation and init
	nodes = getPattern(nmbInitNodes,range_,zeroCentred_);
	nmbNodes = nmbInitNodes;
	// show init activation
	refreshInitWin(nodes,nmbInitNodes);

	
	
	
   // iteration the processes
   for(;;){
     	int c = cvWaitKey(5);  	
     	if(c != -1){		
     	   	  	if(c == 27){
     	   	  		break;
     	   	  	};
     	   	  	
     	   	  	if(c == 'z'){
     	        	// iteration
     	   	  		cout << "update \n";
     	   	  		
     	      		try{	
     	      			if(nmbNodes == 0){
     	      				// make win black
     	      				refreshSystemWin(NULL,0);
     	      			}
     	      			mem.updateActivationWithClosestNodes(nmbNodes,nodes);
     	      			delete (nodes);
     	      			nodes = mem.getActiveNodes(&nmbNodes, actVec);
     	      			// show init activation
     	      			refreshSystemWin(nodes,nmbNodes);
     	        	}catch(IMapException e){
     	        		cout << "Cannot update activation, " << e.getMsg() << ". Terminate process.\n";
     	        		exit(0);
     	        	}catch(...){
     	        		cout << "Cannot update activation, reason unknown. Terminate process.\n";
     	        		exit(0);
     	        	};
     	   	  	 };
     	   	  	 
     	   	  	 if(c == 'r'){
     	   	  		 // init activation and init
     	   	  		 
     	   	  		 cout << "new init \n";
     	   	  		 delete(nodes);
     	   	  		 nodes = getPattern(nmbInitNodes,range_,zeroCentred_);
     	   	  		 nmbNodes = nmbInitNodes;
     	   	  		 // show init activation
     	   	  		 refreshInitWin(nodes,nmbInitNodes);
      				// make system win black
      				refreshSystemWin(NULL,0);
     	   	  	 }
     	   	  	 
     	   	  	 if(c == 'd'){
     	   	  		 // new input
     	   	  		 delete(nodes);
     	   	  		 nodes = getPattern(nmbInitNodes,range_,zeroCentred_);
     	   	  		 nmbNodes = nmbInitNodes;
     	   	  		 // show init activation
     	   	  		 refreshInitWin(nodes,nmbInitNodes);

     	   	  		 
     	   	  		 mem.updateActivationWithClosestNodes(nmbNodes,nodes);
     	   	  		 delete (nodes);
     	   	  		 nodes = mem.getActiveNodes(&nmbNodes, actVec);
     	   	  		 // show init activation
     	   	  		 refreshSystemWin(nodes,nmbNodes);
     	   	  		 
     	   	  	 }
     	   	  	 
     	   	  	 if(c == 'm'){ // create a new map
     	   	  		 // create  a map
     	   	  		 map.deinit();
     	   	  		 map.init(inpDim_,outDim,1,nmbLinks*2);
     	   	  		 if(zeroCentred_){
     	   	  			 createCentredConnections(&map, range_, nmbLinks);
     	   	  		 }else{
     	   	  			 createRandomConnections(&map, range_, nmbLinks);
     	   	  		 } 
     	   	  		 
     	   	  	 };
     	   	  	 
     	   	  	 if(c == 'a'){ // change accumulation mode
     	   	  		 accumulation_ = !accumulation_;
     	   	  		 cout << "accumulationMode = " << accumulation_;
     	   	  		 cout.flush();
     	   	  	 }
     	   	  	 
     	   	  	 if(c == 'c'){ // change centred map
     	   	  		 zeroCentred_ = !zeroCentred_;
     	   	  	 }
     	   	  	
     	};


   };
	
   	
   deinitWindows();
   mem.deinitMap();
   map.deinit();
	

   exit(0);
};

float *getPattern(int nmbNodes, int range, bool zeroCentr){
	float *n = new float[inpDim_*nmbNodes];
	int offSet = -range_ / 2;
	for(int i = 0; i < (nmbNodes*inpDim_); i++){
			// zero centred values 
			if(zeroCentr){
				n[i] = (float)(offSet + ((int)(range_*((float)rand()) / ((float)RAND_MAX))));
			}else{
				n[i] = (float)((int) 
					((range_*((float)rand()) / ((float)RAND_MAX))));
			};
	};
	return n;
}


void refreshInitWin(float *nodes, int nmbNodes){
	// no accumulation
	refreshWindow(openCvInitImage_,initWinName_,nodes,nmbNodes, accumulation_, 25);
}

void refreshSystemWin(float *nodes, int nmbNodes){
	cout << "\n\n refSysWin entering with accu-mode: " << accumulation_ << "  \n\n\n";
	cout.flush();

	
	// via accumulation
	refreshWindow(openCvSystemImage_,systemWinName_,nodes,nmbNodes, accumulation_, 2);
}

void refreshWindow(IplImage *openCvImage, char *windowName, float *nodes, int nmbNodes, bool accu, int accValue){
	uchar  chW;
	uchar  chH;
	int dW,dH;
	float fW, fH;
	int maxVal, val;
	
	
	cout << "\n\n refWin entering with accu-mode: " << accu << "  \n\n\n";
	cout.flush();
	
	// make it black
	for(int i = 0; i < WINDOW_HEIGHT_; i++){
		for(int j=0; j < WINDOW_WIDTH_; j++){
			((uchar *)(openCvImage->imageData + i*openCvImage->widthStep))[j] =
				0;
		};
	};
	
	if((nodes == NULL) || (nmbNodes == 0)){
		cvShowImage(windowName,openCvImage);
		cvWaitKey(2);	
		return;
	};
		
	for(int k=0; k < nmbNodes; k++){
		fW = nodes[(k*inpDim_)];
		fH = nodes[(k*inpDim_) + 1]; // assume 2-dim map
		dW = (long int) (fW);
		dH = (long int) (fH);
		
		if(zeroCentred_){
			dW = dW + (range_ / 2);
			dH = dH + (range_ / 2);
		};
		
		if(dW < 0 ) dW = 0;
		if(dW >= WINDOW_WIDTH_) dW = WINDOW_WIDTH_ - 1;
		if(dH < 0 ) dH = 0;
		if(dH >= WINDOW_HEIGHT_) dH = WINDOW_HEIGHT_ - 1;
		
		if(accu){
			((uchar *)(openCvImage->imageData + (dH*openCvImage->widthStep)))[dW] += accValue;
			val = (((uchar *)(openCvImage->imageData + (dH*openCvImage->widthStep)))[dW] );
			if(val > maxVal) maxVal = val; 
		}else{
			((uchar *)(openCvImage->imageData + (dH*openCvImage->widthStep)))[dW] = 255;
		};
	};
	
	if(accu){
		float norm = (255.0 / ((float ) (maxVal + 1)));
		cout << "maxVal , norm = " << maxVal <<  " ,  " <<norm << endl; cout.flush();
		for(int w = 0; w < WINDOW_WIDTH_; w++){
			for(int h = 0; h < WINDOW_HEIGHT_; h++){
				((uchar *)(openCvImage->imageData + (dH*openCvImage->widthStep)))[dW] =
					((float)(((uchar *)(openCvImage->imageData + (dH*openCvImage->widthStep)))[dW])*norm);
			}
		}
	}else{
		cout << " no normalization done.\n"; cout.flush();
	}
	
	
	
	cvShowImage(windowName,openCvImage);
	cvWaitKey(2);	
}

void initWindows(char *initWinName, char *systemWinName){
	initWinName_ = initWinName;
	systemWinName_ = systemWinName;
		
	cvNamedWindow(initWinName_, 0);
	cvNamedWindow(systemWinName_, 0);
	
	openCvInitImage_ = cvCreateImage(cvSize(WINDOW_WIDTH_,WINDOW_HEIGHT_),IPL_DEPTH_8U,1);
	openCvSystemImage_ = cvCreateImage(cvSize(WINDOW_WIDTH_,WINDOW_HEIGHT_),IPL_DEPTH_8U,1);
};



void deinitWindows(){
	cvDestroyWindow(systemWinName_);
	cvDestroyWindow(initWinName_);
}

void createCentredConnections(CXmlMap *map, int range, int nmbConnections){
	map->clean();
	int inpDim = map->getInpDim();
	int outDim = map->getOutDim();
	if((inpDim < 1) || (outDim < 1)){
		return;
	};
	
	int offSet = -range / 2;
	
	float* inpNode = new float[inpDim];
	float* outNode = new float[outDim];
	for(int i = 0; i < nmbConnections; i++){
		for(int k = 0; k < inpDim; k++){
			inpNode[k] = (float)(offSet + ((range*((float)rand()) / ((float)RAND_MAX))));
	 		outNode[k] = (float)(offSet + ((0.15*range*((float)rand()) / ((float)RAND_MAX))));
	 		/*
	 		if(outNode[k] < 0){
	 			outNode[k] = outNode[k] + ((float)((int)fabs(outNode[k]*0.5)));
	 		}else{
	 			outNode[k] = outNode[k] - ((float)((int)fabs(outNode[k]*0.5)));
	 		};
	 		*/
	 	 };		
	 	 map->addLink(inpNode,outNode);
	 };
}

void createRandomConnections(CXmlMap *map, int range, int nmbConnections){
	map->clean();
	int inpDim = map->getInpDim();
	int outDim = map->getOutDim();
	if((inpDim < 1) || (outDim < 1)){
		return;
	};
	
	float* inpNode = new float[inpDim];
	float* outNode = new float[outDim];
	for(int i = 0; i < nmbConnections; i++){
		for(int k = 0; k < inpDim; k++){
			inpNode[k] = 1.0 + (float)((int)((range-1)*((float)rand()) / ((float)RAND_MAX)));
	 	 };		
	 	 for(int l = 0; l < outDim; l++){
	 		 outNode[l] = 1 + (float)((int)((range-1)*((float)rand()) / ((float)RAND_MAX)));
	 	 };
	 	 map->addLink(inpNode,outNode);
	 };
};



