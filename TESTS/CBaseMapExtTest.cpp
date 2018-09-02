#include <math.h>
#include <iostream>
#include <cstdlib>
#include "CBaseMap.h"

using namespace std;

int main(int argc, char *argv[]){

  int i = 0;
  char* unitName = argv[0];
  bool isFailed = false;

    // indicating test start of this unit
  cout << "\nTesting:\t" << unitName << endl;
  
  CBaseMap map;

  
  
  int inpDim = 3;
  int outDim = 5;
  int attrDim = 7;
  int initSize = 120;
  float inpNode[inpDim];
  float outNode[outDim];
  float attrValue[attrDim];
  
  /** next test */
  i++; // 
  cout << "\t subtest " << i << "\t";
  ///////////////////////////////////////////////////
  try{
	map.init(inpDim,outDim,1,initSize);
    cout << " OK.\n";
  }catch(...){
  	cout << " FAILED. wrong exception\n" ; // both must be the same 
    isFailed = true;
  };
  //////////////////////////////////////////////////
  
  /** next test */
  i++; // 
  cout << "\t subtest " << i << "\t";
  ///////////////////////////////////////////////////
  try{
	map.deinit();
	map.init(inpDim,outDim,1,initSize);
	map.addLink(inpNode,outNode,attrValue);
    cout << " OK.\n";
  }catch(IMapException e){
	  	cout << " FAILED. wrong exception" ;
	  	cout << e.getMsg() << endl;// both must be the same 
	    isFailed = true;
  }catch(...){
  	cout << " FAILED. wrong exception\n" ; // both must be the same 
    isFailed = true;
  };
  //////////////////////////////////////////////////

  /** next test */
  i++; // 
  cout << "\t subtest " << i << "\t";
  ///////////////////////////////////////////////////
  try{
	map.deinit();
	map.init(inpDim,outDim,1,initSize);
	map.addLink(inpNode,outNode,attrValue);
    cout << " OK.\n";
  }catch(IMapException e){
	  	cout << " FAILED. wrong exception" ;
	  	cout << e.getMsg() << endl;// both must be the same 
	    isFailed = true;
  }catch(...){
  	cout << " FAILED. wrong exception\n" ; // both must be the same 
    isFailed = true;
  };
  //////////////////////////////////////////////////

  
  
  /** next test */
  i++; // 
  cout << "\t subtest " << i << "\t";
  ///////////////////////////////////////////////////
  try{
	map.deinit();
	map.init(inpDim,outDim,attrDim,initSize);
    cout << " OK.\n";
  }catch(...){
  	cout << " FAILED. wrong exception\n" ; // both must be the same 
    isFailed = true;
  };
  //////////////////////////////////////////////////

  
  /** next test */
  i++; // 
  cout << "\t subtest " << i << "\t";
  ///////////////////////////////////////////////////
  try{
	map.deinit();
	map.init(inpDim,outDim,attrDim,initSize);
	
	// fill list
	for(int i = 0; i < initSize; i++){
		for(int n=0; n < inpDim; n++){
			inpNode[n] = (float) n;
		};
		for(int m=0; m < outDim; m++){
		  outNode[m] = (float) m;
		};
		for(int a=0; a < attrDim; a++){
			attrValue[a] = (float) a;
		};
		map.addLink(inpNode,outNode,attrValue);
		if(map.nmbOfLinks() != (i+1)){
		  	cout << " FAILED. wrong list size.\n" ; // both must be the same
		    isFailed = true;
		    break;
		};
	};
	  
	// test list content
	float *ptrLink;
	for(int i = 0; i < initSize; i++){
		ptrLink = map.ptrLinkByIndex(i);
		for(int t=0; t < inpDim;t++){
			inpNode[t] = ptrLink[t];
		};	
		for(int t=0; t < outDim;t++){
			outNode[t] = ptrLink[inpDim + t];
		};
		for(int t=0; t < attrDim;t++){
			attrValue[t] = ptrLink[inpDim + outDim + t];
		};
		  
		for(int n=0; n < inpDim; n++){
			if(inpNode[n] != ((float)n)){
				cout << " FAILED. wrong inpNode value at index ";
				cout << n << endl;// both must be the same
				isFailed = true;
				break;
			};
		};
		for(int n=0; n < outDim; n++){
			if(outNode[n] != ((float)n)){
				cout << " FAILED. wrong outNode value at index ";
				cout << n << endl;// both must be the same
				isFailed = true;
				break;
			};
		};
		for(int n=0; n < attrDim; n++){
			if(attrValue[n] != ((float)n)){
				cout << " FAILED. wrong attribute value at index ";
				cout << n << endl;// both must be the same
				isFailed = true;
				break;
			};
		};

		  
	};
	cout << " OK.\n";
  }catch(...){
  	cout << " FAILED. wrong exception\n" ; // both must be the same 
    isFailed = true;
  };
  
  //////////////////////////////////////////////////


  
  /** next test */
  i++; // 
  cout << "\t subtest " << i << "\t";
  ///////////////////////////////////////////////////
  int counter = 0;
  try{
	map.clean();
	
	// fill list
	for(int i = 0; i < initSize; i++){
		for(int n=0; n < inpDim; n++){
			inpNode[n] = (float) counter; counter++;
		};
		for(int m=0; m < outDim; m++){
		  outNode[m] = (float) counter; counter++;
		};
		for(int a=0; a < attrDim; a++){
			attrValue[a] = (float) counter; counter++;
		};
		map.addLink(inpNode,outNode,attrValue);
		if(map.nmbOfLinks() != (i+1)){
		  	cout << " FAILED. wrong list size.\n" ; // both must be the same
		    isFailed = true;
		    break;
		};
	};
	  
	counter = 0;
	// test list content
	float *ptrLink;
	for(int i = 0; i < initSize; i++){
		ptrLink = map.ptrLinkByIndex(i);
		for(int t=0; t < inpDim;t++){
			inpNode[t] = ptrLink[t];
		};	
		for(int t=0; t < outDim;t++){
			outNode[t] = ptrLink[inpDim + t];
		};
		for(int t=0; t < attrDim;t++){
			attrValue[t] = ptrLink[inpDim + outDim + t];
		};
		  
		for(int n=0; n < inpDim; n++){
			if(inpNode[n] != ((float)counter)){
				cout << " FAILED. wrong inpNode value at index ";
				cout << n << endl;// both must be the same
				isFailed = true;
				break;
			};
			counter++;
		};
		for(int n=0; n < outDim; n++){
			if(outNode[n] != ((float)counter)){
				cout << " FAILED. wrong outNode value at index ";
				cout << n << endl;// both must be the same
				isFailed = true;
				break;
			};
			counter++;
		};
		for(int n=0; n < attrDim; n++){
			if(attrValue[n] != ((float)counter)){
				cout << " FAILED. wrong attribute value at index ";
				cout << n << endl;// both must be the same
				isFailed = true;
				break;
			};
			counter++;
		};

		  
	};
	
	int expectedValueNmb = (initSize*(inpDim + outDim + attrDim)); 
	if(counter != expectedValueNmb){
		cout << " FAILED. total value number  ";
		cout <<  counter << " != " << expectedValueNmb << endl; 
		isFailed = true;		
	};
	
	//cout << "\t\t\t\t\t " << counter << "endl";
	cout << " OK.\n";
  }catch(...){
  	cout << " FAILED. wrong exception\n" ; // both must be the same 
    isFailed = true;
  };
  
  //////////////////////////////////////////////////

  
  
  
  // ENDE TESTING ///  
  if(isFailed){
    // some test(s) failed.
    cout << "ERROR!!! Some tests of unit " << unitName << " failed. \n\n";
  }else{
    // closing all test successfully done.
    cout << "unit " << unitName << " successfully tested.\n\n";
  }
  
  
  exit(0);
};
