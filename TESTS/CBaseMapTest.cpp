#include <math.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "CBaseMap.h"

using namespace std;

int main(int argc, char *argv[]){

  int i = 0;
  char* unitName = argv[0];
  bool isFailed = false;

    // indicating test start of this unit
  cout << "\nTesting:\t" << unitName << endl;
  
  CBaseMap map;

 

  /** next test */
  i++; // 
  cout << "\t subtest " << i << "\t";
  ///////////////////////////////////////////////////
  if(map.nmbOfLinks() == 0){
    cout << " OK.\n";
  }else{
  	cout << " FAILED.\n" ; // both must be the same 
    isFailed = true;
  };
  //////////////////////////////////////////////////

  
  
  
  /** next test */
  i++; // 
  cout << "\t subtest " << i << "\t";
  ///////////////////////////////////////////////////
  try{
	map.init(0,0,0,0);
  	cout << " FAILED. no exception at all\n" ; // both must be the same 
    isFailed = true;
  }catch(IMapException){
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
	map.init(2,4,0,0);
  	cout << " FAILED. no exception at all\n" ; // both must be the same 
    isFailed = true;
  }catch(IMapException){
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
	map.init(2,0,0,10);
  	cout << " FAILED. no exception at all\n" ; // both must be the same 
    isFailed = true;
  }catch(IMapException){
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
	map.init(0,1,0,10);
  	cout << " FAILED. no exception at all\n" ; // both must be the same 
    isFailed = true;
  }catch(IMapException){
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
	map.init(3,1,1,10);
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
  if(map.nmbOfLinks() == 0){
    cout << " OK.\n";
  }else{
  	cout << " FAILED.\n" ; // both must be the same 
    isFailed = true;
  };
  //////////////////////////////////////////////////

  
  /** next test */
  i++; // 
  cout << "\t subtest " << i << "\t";
  ///////////////////////////////////////////////////
  try{
	map.clean();
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
    cout << " OK.\n";
  }catch(...){
  	cout << " FAILED. wrong exception\n" ; // both must be the same 
    isFailed = true;
  };
  //////////////////////////////////////////////////

  int inpDim = 3;
  int outDim = 5;
  int attrDim = 11;
  int initSize = 120;
  float inpNode[inpDim];
  float outNode[outDim];
  
  /** next test */
  i++; // 
  cout << "\t subtest " << i << "\t";
  ///////////////////////////////////////////////////
  try{
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
  int capacity = 8827;
  try{
	  for(int n=0; n < capacity; n++){
		map.addLink(inpNode,outNode);  
	  };
	  
	  int counter = 0;
	  do{
		  map.removeLink(0);
		  counter++;
	  }while(map.nmbOfLinks() != 0);
	  if(counter == capacity){
		  cout << " OK.\n";
	  }else{
	  	cout << " FAILED. wrong number of deleted nodes\n" ;
	  	cout << " capacity = " << capacity << " != " << counter << endl;
	    isFailed = true;
	  };
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
	map.addLink(inpNode,outNode);
  	cout << " FAILED. no exception at all\n" ; // both must be the same 
    isFailed = true;
  }catch(IMapException){
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
	map.init(inpDim,outDim,10,initSize);
	map.addLink(inpNode,outNode);
    cout << " OK.\n";
  }catch(...){
  	cout << " FAILED. exception raised\n" ; // both must be the same 
    isFailed = true;
  };
  //////////////////////////////////////////////////


  
  /** next test */
  i++; // 
  cout << "\t subtest " << i << "\t";
  ///////////////////////////////////////////////////
  try{
	int c = 13;
	map.clean();
	for(int i = 0; i < c; i++){
		map.addLink(inpNode,outNode);
	};
	
	if(map.nmbOfLinks() == c){
		cout << " OK.\n";
	}else{
	  	cout << " FAILED. wron nmb of elements.\n" ; // both must be the same 
	    isFailed = true;
	};
  }catch(...){
  	cout << " FAILED. exception raised\n" ; // both must be the same 
    isFailed = true;
  };
  //////////////////////////////////////////////////

  /** next test */
  i++; // 
  cout << "\t subtest " << i << "\t";
  ///////////////////////////////////////////////////
  try{
	map.clean();
	if(map.nmbOfLinks() == 0){
		cout << " OK.\n";
	}else{
	  	cout << " FAILED. wron nmb of elements.\n" ; // both must be the same 
	    isFailed = true;
	};
  }catch(...){
  	cout << " FAILED. exception raised\n" ; // both must be the same 
    isFailed = true;
  };
  //////////////////////////////////////////////////

  
  /** next test */
  i++; // 
  cout << "\t subtest " << i << "\t";
  ///////////////////////////////////////////////////
  try{
	int nmb = 13*initSize;
	map.clean();
	for(int i = 0; i < nmb; i++){
		map.addLink(inpNode,outNode);
	};
	
	if(map.nmbOfLinks() == nmb){
		cout << " OK. (size: " << nmb << ")\n";
	}else{
	  	cout << " FAILED. wron nmb of elements.\n" ; // both must be the same 
	    isFailed = true;
	};
  }catch(...){
  	cout << " FAILED. exception raised\n" ; // both must be the same 
    isFailed = true;
  };
  //////////////////////////////////////////////////

  int nmb = 231;
  
  /** next test */
  i++; // 
  cout << "\t subtest " << i << "\t";
  ///////////////////////////////////////////////////
  try{
	int p =0;
	map.clean();
	for(int i = 0; i < (nmb*initSize); i++){
		for(int k = 0; k < inpDim;k++){
			inpNode[k] = (float)(p);
			p++;
		};
		for(int l = 0; l < outDim;l++){
			outNode[l]=p;
			p++;
		};
		try{
			map.addLink(inpNode,outNode);
		}catch(...){
			cout << "to less memory, terminate test" << endl;
			exit(-1);
		};
	};
	
	if(map.nmbOfLinks() == nmb*initSize){
		cout << " OK. (size: " << map.nmbOfLinks() << ")\n";
	}else{
	  	cout << " FAILED. wron nmb of elements.\n" ; // both must be the same 
	    isFailed = true;
	};
  }catch(...){
  	cout << " FAILED. exception raised\n" ; // both must be the same 
    isFailed = true;
  };
  //////////////////////////////////////////////////
  map.deinit();
  map.init(inpDim, outDim, attrDim, 10);

  float resultOut[outDim];
  float resultInp[inpDim];
  
  /** next test */
   i++; // 
   cout << "\t subtest " << i << "\t";
   ///////////////////////////////////////////////////
   map.clean();
   
   for(int k = 0; k < inpDim; k++){
	   inpNode[k] = (float) k;
   };
   for(int l = 0; l < outDim; l++){
	   outNode[l] = (float) l;
   };

   try{
	   map.addLink(inpNode,outNode);
   }catch(IMapException e){
	 cout << "preparation: " << e.getMsg() << endl;
   };
   
   float *link;
   for(int i=0; i< map.nmbOfLinks(); i++){
	   link = map.ptrLinkByIndex(i);
   }

   float sum = 0.0;
   try{
	   map.getOutputNode(inpNode,resultOut);
	   for(int j = 0; j < outDim;j++){
		   sum = sum + fabs(resultOut[j] - outNode[j]);
	   };
	   
	}catch(IMapException e){
	   	cout << " FAILED. excp. while reading Input Node. terminate test.\n" ; // both must be the same
	   	cout << e.getMsg() << endl;
	   	isFailed = true;
	   	exit(-1);
	};
 	
 	if(sum == 0){
 		cout << " OK. \n";
 	}else{
 		cout << sum << endl;
 	  	cout << " FAILED. write and read different.\n" ; // both must be the same 
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
