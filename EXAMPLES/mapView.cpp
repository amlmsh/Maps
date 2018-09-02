#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include <iostream>
#include <stdexcept>
#include <math.h>
#include <float.h>
//opencv 
#include <opencv2/opencv.hpp>

#include "CXmlMap.h"

bool inputSpace_;
int idxX_;
int idxY_;
int dim_;
int size_;
int MAX_DATA_;
int MIN_DATA_;
int INDEX_;
float minX_, maxX_, minY_, maxY_;
static float *ptrX_;
static float *ptrY_; 

// views
IplImage *openCvImage_;
static char *winName_ ;
int WINDOW_WIDTH_ = 100;
int WINDOW_HEIGHT_ = 100;

	
using namespace std;

XERCES_CPP_NAMESPACE_USE

int info(int argc, char* args[]);
int readCommandLine(int argc, char* args[], CXmlMap *map);
int readData(CXmlMap *map);
void initWindow();
void refreshWindow();
void deinitWindow();
void clearDisplay();



int main (int argc, char* args[]) {
	if(info(argc,args) != 0){ exit(0);};
	
	CXmlMap *map = new CXmlMap();
	try{
		map->readXmlFileWithoutAttributes(args[1]);
	}catch(...){
		cout << "cannot read file : \"" << args[1] << "\" terminate process.\n";
		cout.flush();
		exit(0);
	};

	
	readCommandLine(argc,args,map);
	// everything is checked now
	readData(map);	
	
	//exit(0);

	winName_ = args[1];
	initWindow();

	// iteration the processes
	for(;;){
	  	int c = cvWaitKey(5);  	
	  	if(c != -1){		
	      	if(c == 27){
	      		break;
	      	};
	  	}
	  	
	  	refreshWindow();
	 };
	
	delete ptrX_;
	delete ptrY_;
	delete winName_;
	deinitWindow();
	
	

};


void refreshWindow(){
	float normX = (maxX_ - minX_);
	float normY = (maxY_ - minY_);
	
	clearDisplay();
	for(int k=MIN_DATA_; k < MAX_DATA_; k++){
		float fX = ptrX_[k];
		float fY = ptrY_[k];
		
		//cout << "fX: " << fX << " , " << "fY: " << fY << endl;
		
		fX = (fX - minX_) * ((float) WINDOW_WIDTH_);
		fX = fX / normX;
		fY = (fY - minY_) * ((float) WINDOW_HEIGHT_);
		fY = fY / normY;
		
		
		
		long int dW = (long int) (fX);
		long int dH = (long int) (fY);
		
		if(dW < 0) dW = 0;
		if(dW > (WINDOW_WIDTH_-1)) dW = (WINDOW_WIDTH_-1); 
		if(dH < 0) dH = 0;
		if(dH > (WINDOW_HEIGHT_-1)) dH = (WINDOW_HEIGHT_-1); 
		
		((uchar *)(openCvImage_->imageData + (dH*openCvImage_->widthStep)))[dW] = 255;
		
		if(k == INDEX_){ // draw cicle
			cvCircle(openCvImage_, cvPoint(dW,dH), 3, cvScalar(255), 1);
		};
		
	};
	
	cvShowImage(winName_,openCvImage_);
	cvWaitKey(2);
};


void clearDisplay(){
	int m = WINDOW_WIDTH_*WINDOW_HEIGHT_;
	for(int p = 0; p < m; p++){
			openCvImage_->imageData[p] = 0;
	}
}

void initWindow(){
	cvNamedWindow(winName_, 0);	
	cvCreateTrackbar("Start Idx >",winName_, &MIN_DATA_, size_, NULL );
	cvCreateTrackbar("End Idx >",winName_, &MAX_DATA_, size_, NULL );
	cvCreateTrackbar("Idx >",winName_, &INDEX_, size_, NULL );
	openCvImage_ = cvCreateImage(cvSize(WINDOW_WIDTH_,WINDOW_HEIGHT_),IPL_DEPTH_8U,1);
};

void deinitWindow(){
	cvDestroyWindow(winName_);
};

int readData(CXmlMap *map){
	
	float *link = NULL;
	float *node = NULL;
	
	try{
		ptrX_ = new float[size_];
		ptrY_ = new float[size_];
		cout << "size: " << size_ << endl;
	}catch(...){
		cout << "No memory to store map values, terminate process.";
		exit(0);
	};

	

	
	for(int i= 0; i < size_; i++){
		link = map->ptrLinkByIndex(i);
		
		if(inputSpace_){
			node = &(link[0]);
		}else{
			node = &(link[map->getInpDim()]);
		};
		
		ptrX_[i] = node[idxX_];
		ptrY_[i] = node[idxY_];
		
		if(i == 0){
			minX_ = ptrX_[i];
			maxX_ = ptrX_[i];

			minY_ = ptrY_[i];
			maxY_ = ptrY_[i];
		}else{			
			if(ptrX_[i] > maxX_) maxX_ = ptrX_[i];
			if(ptrY_[i] > maxY_) maxY_ = ptrY_[i];
		
			if(ptrX_[i] < minX_) minX_ = ptrX_[i];
			if(ptrY_[i] < minY_) minY_ = ptrY_[i];
		};
	};
	
	if((minX_ - maxX_) == 0.0){
		minX_ = minX_ - 0.01;
		maxX_ = maxX_ + 0.01;
	};
	
	if((minY_ - maxY_) == 0.0){
		minY_ = minY_ - 0.01;
		maxY_ = maxY_ + 0.01;
	};
	
	
}

int readCommandLine(int argc, char* args[], CXmlMap *map){
	

	if(atoi(args[2]) == 0){
		inputSpace_ = true;
	}else{
		inputSpace_ = false;
	};
	

	if(inputSpace_){
		dim_ = map->getInpDim();
	}else{
		dim_ = map->getOutDim();
	};
	if(dim_ < 2){
		cout << "Dimension is less than 2, which cannot be projected. Terminate process.\n";
		cout.flush();
		exit(0);
	}; 
	
	try{
		idxX_ = atoi(args[3]);
		idxY_ = atoi(args[4]);
	}catch(...){
		cout << "Cannot read command line parameter, please check.\n";
		cout.flush();
		exit(0);
	};
	
	if((idxX_ < 0) || (idxX_ > (dim_-1))){
		cout << "First component index is out of range for the given map. ";
		cout << "Terminate process.\n";
		cout.flush();
		exit(0);		
	};
	if((idxY_ < 0) || (idxY_ > (dim_-1))){
		cout << "Second component index is out of range for the given map. ";
		cout << "Terminate process.\n";
		cout.flush();
		exit(0);		
	};
	
	size_ = map->nmbOfLinks();
	if(size_ < 1){
		cout << "The given map is empty, no projections possible. ";
		cout << "Terminate process.\n";
		cout.flush();
		exit(0);				
	};
	
}

int info(int argc, char* args[]){
	
	if(argc == 5) return (0);
	
	cout << "the use of " << args[0] << " :\n";
	cout << "" << args[0] <<  " <xml-file> <0/1> <n>  <m> ";
	cout << "\t <xml-filename> ... xml file name of the map\n";
	cout << "\t <0/1>          ... determines the projected space 0 -> input, 1 -> output\n";
	cout << "\t <n> <m>        ... index of the compenents projected in the 2-dimensional image.\n";
	return(-1);
};

