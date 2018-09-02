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
bool inputSpaceRef_;
int idxX_;
int idxY_;
int idxXRef_;
int idxYRef_;
int dim_, dimRef_;
int size_;
int MAX_DATA_;
int MIN_DATA_;
int INDEX_;
float minX_, maxX_, minY_, maxY_;
float minRefX_, maxRefX_, minRefY_, maxRefY_;
static float *ptrX_;
static float *ptrY_;

static float *ptrRefX_;
static float *ptrRefY_; 


// views
IplImage *openCvImage_;
static char *winName_ ;
int WINDOW_WIDTH_ = 100;
int WINDOW_HEIGHT_ = 100;

int COMPLETE_VIEW_WIDTH_ = 2*WINDOW_WIDTH_ + (0.05 * WINDOW_WIDTH_);
	
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
	delete ptrRefX_;
	delete ptrRefY_;
	delete winName_;
	deinitWindow();
	
	

};


void refreshWindow(){
	float normX = (maxX_ - minX_);
	float normY = (maxY_ - minY_);
	
	float normRefX = (maxRefX_ - minRefX_); 
	float normRefY = (maxRefY_ - minRefY_);
	
	//color norm 
	float r,g,b;
	
	clearDisplay();
	for(int k=MIN_DATA_; k < MAX_DATA_; k++){
		float fX = ptrX_[k];
		float fY = ptrY_[k];
		
		float fXRef = ptrRefX_[k];
		float fYRef = ptrRefY_[k];
		
		//cout << "fX: " << fX << " , " << "fY: " << fY <<  
		// "fXref: " << fXRef << " , " << "fYref: " << fYRef <<  endl;
		
		fX = (fX - minX_) * ((float) WINDOW_WIDTH_);
		fX = fX / normX;
		fY = (fY - minY_) * ((float) WINDOW_HEIGHT_);
		fY = fY / normY;
		

		fXRef = (fXRef - minRefX_) * ((float) WINDOW_WIDTH_);
		fXRef = fXRef / normRefX;
		fYRef = (fYRef - minRefY_) * ((float) WINDOW_HEIGHT_);
		fYRef = fYRef / normRefY;

		
		long int dW = (long int) (fX);
		long int dH = (long int) (fY);

		long int dWRef = (long int) (fXRef) + (COMPLETE_VIEW_WIDTH_ - WINDOW_WIDTH_) ;
		long int dHRef = (long int) (fYRef);

		
		if(dW < 0) dW = 0;
		if(dW > (WINDOW_WIDTH_-1)) dW = (WINDOW_WIDTH_-1); 
		if(dH < 0) dH = 0;
		if(dH > (WINDOW_HEIGHT_-1)) dH = (WINDOW_HEIGHT_-1); 

		if(dWRef < 0) dW = 0;
		if(dWRef > (COMPLETE_VIEW_WIDTH_-1)) dWRef = (COMPLETE_VIEW_WIDTH_-1); 
		if(dHRef < 0) dH = 0;
		if(dHRef > (WINDOW_HEIGHT_-1)) dHRef = (WINDOW_HEIGHT_-1); 

		// color normalization
		r = ((float)dW / (float)WINDOW_WIDTH_)*255.0;
		g = ((float)dH / (float)WINDOW_HEIGHT_)*255.0;
		b = 255.0 - r;
		
		
		//((uchar *)(openCvImage_->imageData + (dH*openCvImage_->widthStep)))[dW] = 255;
		((uchar *)(openCvImage_->imageData + (dH*openCvImage_->widthStep)))[dW*openCvImage_->nChannels + 0] = (uchar)r; //255;
		((uchar *)(openCvImage_->imageData + (dH*openCvImage_->widthStep)))[dW*openCvImage_->nChannels + 1] = (uchar)g; //255;
		((uchar *)(openCvImage_->imageData + (dH*openCvImage_->widthStep)))[dW*openCvImage_->nChannels + 2] = (uchar)b; //255;
		
		((uchar *)(openCvImage_->imageData + (dHRef*openCvImage_->widthStep)))[dWRef*openCvImage_->nChannels + 0] = (uchar)r; //255;
		((uchar *)(openCvImage_->imageData + (dHRef*openCvImage_->widthStep)))[dWRef*openCvImage_->nChannels + 1] = (uchar)g; //255;
		((uchar *)(openCvImage_->imageData + (dHRef*openCvImage_->widthStep)))[dWRef*openCvImage_->nChannels + 2] = (uchar)b; //255;

		
		if(k == INDEX_){ // draw cicle
			cvCircle(openCvImage_, cvPoint(dW,dH), 3, cvScalar(255,100,100), 1);
			cvCircle(openCvImage_, cvPoint(dWRef,dHRef), 3, cvScalar(255,100,100), 1);
		};
		
	};
	
	cvShowImage(winName_,openCvImage_);
	cvWaitKey(2);
};


void clearDisplay(){
	int m = WINDOW_WIDTH_*COMPLETE_VIEW_WIDTH_*3;
	for(int p = 0; p < m; p++){
			openCvImage_->imageData[p] = 0;
	}
	for(int i = WINDOW_WIDTH_; 
		i < (COMPLETE_VIEW_WIDTH_ - WINDOW_WIDTH_);  i++ ){
		for(int j = 0; j < WINDOW_HEIGHT_; j++){
			((uchar *)(openCvImage_->imageData + (j*openCvImage_->widthStep)))[i*openCvImage_->nChannels + 0] = 255;
			((uchar *)(openCvImage_->imageData + (j*openCvImage_->widthStep)))[i*openCvImage_->nChannels + 1] = 255;
			((uchar *)(openCvImage_->imageData + (j*openCvImage_->widthStep)))[i*openCvImage_->nChannels + 2] = 255;
		};
	};
}

void initWindow(){
	cvNamedWindow(winName_, 0);	
	cvCreateTrackbar("Start Idx >",winName_, &MIN_DATA_, size_, NULL );
	cvCreateTrackbar("End Idx >",winName_, &MAX_DATA_, size_, NULL );
	cvCreateTrackbar("Idx >",winName_, &INDEX_, size_, NULL );
	openCvImage_ = cvCreateImage(cvSize(COMPLETE_VIEW_WIDTH_,WINDOW_HEIGHT_),IPL_DEPTH_8U,3);
};

void deinitWindow(){
	cvDestroyWindow(winName_);
};

int readData(CXmlMap *map){
	
	float *link = NULL;
	float *node = NULL;

	float *nodeRef = NULL;
	
	try{
		ptrX_ = new float[size_];
		ptrY_ = new float[size_];
		ptrRefX_ = new float[size_];
		ptrRefY_ = new float[size_];
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
		
		if(inputSpaceRef_){
			nodeRef = &(link[0]);
		}else{
			nodeRef = &(link[map->getInpDim()]);
		};
		
		
		ptrX_[i] = node[idxX_];
		ptrY_[i] = node[idxY_];
		ptrRefX_[i] = nodeRef[idxXRef_];
		ptrRefY_[i] = nodeRef[idxYRef_];
			
		
		
		if(i == 0){
			minX_ = ptrX_[i];
			maxX_ = ptrX_[i];

			minY_ = ptrY_[i];
			maxY_ = ptrY_[i];

			minRefX_ = ptrRefX_[i];
			maxRefX_ = ptrRefX_[i];

			minRefY_ = ptrRefY_[i];
			maxRefY_ = ptrRefY_[i];

			
			
		}else{			
			if(ptrX_[i] > maxX_) maxX_ = ptrX_[i];
			if(ptrY_[i] > maxY_) maxY_ = ptrY_[i];
		
			if(ptrX_[i] < minX_) minX_ = ptrX_[i];
			if(ptrY_[i] < minY_) minY_ = ptrY_[i];
			
			if(ptrRefX_[i] > maxRefX_) maxRefX_ = ptrRefX_[i];
			if(ptrRefY_[i] > maxRefY_) maxRefY_ = ptrRefY_[i];
		
			if(ptrRefX_[i] < minRefX_) minRefX_ = ptrRefX_[i];
			if(ptrRefY_[i] < minRefY_) minRefY_ = ptrRefY_[i];
		};
	};
	
	
	if((minX_ - maxX_) == 0.0)  {
		minX_ = minX_ - 0.01;
		maxX_ = maxX_ + 0.01;
	};
	if((minY_ - maxY_) == 0.0)  {
		minY_ = minY_ - 0.01;
		maxY_ = maxY_ + 0.01;
	}
		
	if((minRefX_ - maxRefX_) == 0.0){
		minRefX_ = minRefX_ - 0.01;
		maxRefX_ = maxRefX_ + 0.01;
	}
	
	if((minRefY_ - maxRefY_) == 0.0){
		minRefY_ = minRefY_ - 0.01;
		maxRefY_ = maxRefY_ + 0.01;
	}
	
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

	
	if(atoi(args[5]) == 0){
		inputSpaceRef_ = true;
	}else{
		inputSpaceRef_ = false;
	};

	if(inputSpaceRef_){
		dimRef_ = map->getInpDim();
	}else{
		dimRef_ = map->getOutDim();
	};
	if(dim_ < 2){
		cout << "Dimension is less than 2, which cannot be projected. Terminate process.\n";
		cout.flush();
		exit(0);
	}; 

	
	try{
		idxXRef_ = atoi(args[6]);
		idxYRef_ = atoi(args[7]);
	}catch(...){
		cout << "Cannot read command line parameter, please check.\n";
		cout.flush();
		exit(0);
	};
	
	
	cout << idxXRef_ << " : " << idxYRef_ << endl;

	
	if((idxXRef_ < 0) || (idxXRef_ > (dimRef_-1))){
		cout << "First component index is out of range for the given map. ";
		cout << "Terminate process.\n";
		cout.flush();
		exit(0);		
	};
	if((idxYRef_ < 0) || (idxYRef_ > (dimRef_-1))){
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
	
	if(argc == 8) return (0);
	
	cout << "the use of " << args[0] << " :\n";
	cout << "" << args[0] <<  " <xml-file> <1:0/1> <1:n>  <1:m> <2:0/1> <2:n>  <2:m>";
	cout << "\t <xml-filename> ... xml file name of the map\n";
	cout << "\t <1:0/1>          ... 1. view determines the projected space 0 -> input, 1 -> output\n";
	cout << "\t <1:n> <1:m>        ... 1. view index of the compenents projected in the 2-dimensional image.\n";
	cout << "\t <2:0/1>          ... 2. view determines the projected space 0 -> input, 1 -> output\n";
	cout << "\t <2:n> <2:m>        ... 2. view index of the compenents projected in the 2-dimensional image.\n";
	
	return(-1);
};

