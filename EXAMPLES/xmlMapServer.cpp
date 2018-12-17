/*
 * xmlMapServer.cpp
 *
 *  Created on: 16.12.2018
 *      Author: aml
 */

/*
 * fillMap.cpp
 *
 *  Created on: 14.12.2018
 *      Author: aml
 */

#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>

#include <algorithm>
#include <iterator>
#include <cassert>


#include "CXmlMap.h"
#include "Socket.H"

XERCES_CPP_NAMESPACE_USE

unsigned short SERVER_PORT_;
char *XML_FILE_NAME_;
CXmlMap *map_;
TCPServerSocket *server_ = NULL;
unsigned int RCVBUFSIZE_;    // Size of receive buffer
unsigned int SNDBUFSIZE_;
int INP_DIM_;
int OUT_DIM_;
float *inpData_ = NULL;
float *inpDataTmp_ = NULL;
float *outData_ = NULL;
char  *sndData_ = NULL;
char  *rcvData_ = NULL;
int   sizePerFloat_ = 15;


using namespace std;


void info(int argc, char* args[]);
void HandleTCPClient(TCPSocket *sock);

void createDataToSnd(int inpDim, int outDim, float *inpA, float *outA);
void getRcvData(string rcvData, int inpDim, float *inpA);

int main(int argc, char* argv[]){


	if(argc != 3){info(argc,argv); exit (0);};


	try{  // read command line parameters
	  SERVER_PORT_   = (unsigned short) atoi(argv[1]);
	  XML_FILE_NAME_ = argv[2];
	}catch(...){
		cerr << "Can't read all parameter values, terminate program.\n\n";
		exit(0);
	};


	try { // read xml file
		map_ = new CXmlMap();
		map_->readXmlFile(XML_FILE_NAME_);
		INP_DIM_ = map_->getInpDim();
		OUT_DIM_ = map_->getOutDim();

		RCVBUFSIZE_ = INP_DIM_*sizePerFloat_;
		SNDBUFSIZE_ = (INP_DIM_ + OUT_DIM_)*sizePerFloat_;
		inpData_ = new float[INP_DIM_];
		inpDataTmp_ = new float[INP_DIM_];
		outData_ = new float[OUT_DIM_];
		rcvData_ =  (char *)malloc(sizeof(char)*RCVBUFSIZE_);
		sndData_ =  (char *)malloc(sizeof(char)*SNDBUFSIZE_);
	}catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	};

	cout << "mapping dimension read: " << INP_DIM_ <<  "(inp. dim.) " << OUT_DIM_ << "(out.dim.)\n";

	try { // init server
		server_ = new TCPServerSocket(SERVER_PORT_); // Server Socket object
	}catch (SocketException &e) {
		delete server_;
		cerr << e.what() << endl;
		exit(1);
	};

	try{
		do{   // Run forever
			HandleTCPClient(server_->accept());       // Wait for a client to connect
	    }while(1);
	}catch (SocketException &e) {
	    cerr << e.what() << endl;
	}catch(IMapException *e){
		cout << "error while handling map data: " << e->getMsg() << endl;
	}

	return 0;
}


void HandleTCPClient(TCPSocket *sock){
  cout << "Handling central unit\n";
  try {
    cout << sock->getForeignAddress() << ":";
  } catch (SocketException &e) {
    cerr << "Unable to get foreign address" << endl;
  }
  try {
    cout << sock->getForeignPort();
  } catch (SocketException &e) {
    cerr << "Unable to get foreign port" << endl;
  }
  cout << endl;

  int recvMsgSize;

  do{
    try{
    	recvMsgSize = sock->recv(rcvData_, RCVBUFSIZE_);
    	if(recvMsgSize < 1) break;
    }catch(SocketException e){
    	cout << "SocketException:" << e.what() << "\n";
    	break;
    }catch(...){
    	cout << "nothing received, went on\n";
    	break;
    };

    cout << "received string: '" << rcvData_ << "'\n";

    // interpret float received
    getRcvData(string(rcvData_), INP_DIM_,inpData_);


    try{
    	// get output data
    	map_->getClosestInputNodeLink(inpData_,inpDataTmp_, outData_);
    }catch(IMapException e){
    	cout << "error while reading ma data: " << e.getMsg() << endl;
    }catch(...){
    	cout << "unknown error while reading map-data\n";
    }

    // send input and output data
    createDataToSnd(INP_DIM_, OUT_DIM_, inpDataTmp_, outData_);

    sndData_[SNDBUFSIZE_-1]='\0';

    cout << "data to be sent: '" << sndData_ << "'\n";

    sock->send(sndData_, SNDBUFSIZE_);
  }while(true);

  delete sock;

};

/*
 * Result will be stored in the global array of char sndData_
 *
 */
void createDataToSnd(int inpDim, int outDim, float *inpA, float *outA){
    stringstream ss;
    ss << std::fixed;
    ss.precision(4);
    for(int i=0; i < INP_DIM_; i++){
    	ss << inpA[i] << "\t";
    }
    for(int i=0; i < (OUT_DIM_-1); i++){
    	ss << outA[i] << "\t";
    }
    ss << outA[OUT_DIM_-1] << "\0";
    sndData_[0]='\0';
    strncpy(sndData_, (ss.str()).c_str(), SNDBUFSIZE_);
    return;
}

void getRcvData(string rcvData, int inpDim, float *inpA){
	// If possible, always prefer std::vector to naked array
	vector<float> values;

	// Build an istream that holds the input string
	istringstream iss(rcvData);

	 // Iterate over the istream, using >> to grab floats
	 // and push_back to store them in the vector
	 copy(istream_iterator<float>(iss),istream_iterator<float>(),back_inserter(values));

	 // collect data
	 int detectedFloats = values.size();
	 if (detectedFloats !=  inpDim){
		string msg("conflicting dimensions while reading values from  line.  We have \n");
		msg += detectedFloats + " reads and while expected dimension is ";
		msg += inpDim + "\n";
		throw new IMapException(msg,-1);
	}

	for(int idx = 0; idx < inpDim; idx++){
		inpA[idx] = values[idx];
	}

	return;
}


void info(int argc, char* args[]){
	cout << "\n usage of " << args[0] << " portNmb   resultMap.xml\n\n";
	cout << "  portNmb   ... port number of the server\n";
	cout << "  map.xml   ... file name of the map\n \n";
	return;
}


