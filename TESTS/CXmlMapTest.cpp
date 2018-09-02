#include <math.h>
#include <iostream>
#include <cstdlib>
#include "CXmlMap.h"

using namespace std;

int main(int argc, char *argv[]){

  srand(time(NULL));
	
  int i = 0;
  char* unitName = argv[0];
  bool isFailed = false;

    // indicating test start of this unit
  cout << "\nTesting:\t" << unitName << endl;
  
  CXmlMap map;

 

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
	  map.readXmlFile("NameDoesNotExit.xml");
	  cout << " FAILED.\n" ; // wrong exception
	  isFailed = true;
  }catch(IMapException e){
    cout << " OK.\n";  // right exception
  }catch(...){
  	cout << " FAILED.\n" ; // wrong exception
    isFailed = true;
  };
  //////////////////////////////////////////////////
  

  
  /** next test */
  i++; // 
  cout << "\t subtest " << i << "\t";
  ///////////////////////////////////////////////////
  int nmbNodes = 3;
  int inpDim = 4;
  int outDim = 9;
  int attrDim = 3;
  float* inpNode= new float [inpDim];
  float* outNode= new float [outDim];
  float* attrValue = new float [attrDim];
  map.init(inpDim,outDim,attrDim,150);
  for(int i = 0; i < nmbNodes; i++){
	  
	  for(int k = 0; k < inpDim; k++){
		  if(i == (int)(nmbNodes / 3)){
			  inpNode[k] = k;
		  }else{
			  inpNode[k] = rand();
		  };
	  }
	  for(int l = 0; l < outDim; l++){
		  if(i == (int)(nmbNodes / 3)){
			  outNode[l] = l;
		  }else{
			  outNode[l] = rand();
		  };
	  };
	  
	  
	  map.addLink(inpNode,outNode);
  };
  map.writeXmlFile("map.xml");
  
  
  try{	  
	 map.readXmlFile("map.xml");
	  if(nmbNodes == map.nmbOfLinks()){
		    cout << " OK.\n";
	  }else{
		 cout << "wrong nmb: " << nmbNodes <<
		 	" != " << map.nmbOfLinks() << endl;  
	  };
	  

		  ;;
  }catch(IMapException e){
	  	cout << e.getMsg() << endl;
	  	cout << " FAILED.\n" ; // wrong exception
	    isFailed = true;
  }catch(...){
  	cout << " FAILED.\n" ; // wrong exception
    isFailed = true;
  };
  //////////////////////////////////////////////////



  /** next test */
  i++; // 
  cout << "\t subtest " << i << "\t";
  ///////////////////////////////////////////////////
  for(int i = 0; i < outDim; i++){
	  outNode[i] = i;
  };
  for(int i = 0; i < inpDim; i++){
	  inpNode[i] = i;
  }
  
  try{
	  
	  map.getInputNode(outNode,inpNode);
	  for(int i = 0; i < inpDim; i++){
		  if(inpNode[i] != (float) i){
			  cout << " FAILED.\n" ; // wrong exception
			  isFailed = true;
			  break;
		  };
	  };
	  cout << " OK.\n";  // right exception
  }catch(IMapException e){
	  cout << " FAILED.\n" ; // wrong exception
	  isFailed = true;
  }catch(...){
  	cout << " FAILED.\n" ; // wrong exception
    isFailed = true;
  };
  //////////////////////////////////////////////////
 
  /** next test */
   i++; // 
   cout << "\t subtest " << i << "\t";
   ///////////////////////////////////////////////////
   nmbNodes = 4;
   inpDim = 2;
   outDim = 3;
   attrDim = 7;
   delete inpNode;
   delete outNode;
   delete attrValue;
   inpNode = new float[inpDim];
   outNode = new float[outDim];
   attrValue = new float[attrDim];
   map.deinit();
   map.init(inpDim,outDim,attrDim,150);
   srand ( time(NULL) );
   for(int i = 0; i < nmbNodes; i++){
 	  
 	  for(int k = 0; k < inpDim; k++){
 		  inpNode[k] = (float)((int)(1000*(((float)rand()) / ((float)RAND_MAX))));
 	  };
 	  for(int l = 0; l < outDim; l++){
 			  outNode[l] = (float)((int)(1000*(((float)rand()) / ((float)RAND_MAX))));
 	  };
 	  for(int m = 0; m < attrDim; m++){
 			  attrValue[m] = (float)((int)(1000*(((float)rand()) / ((float)RAND_MAX))));
 	  };
 	  
 	  map.addLink(inpNode,outNode,attrValue);
   };
   
   map.writeXmlFile("map.xml");
   CXmlMap cpyMap;
   cpyMap.readXmlFile("map.xml");
   cpyMap.writeXmlFile("cpyMap.xml");
   float *inpNodeCpy = new float[inpDim];
   float *outNodeCpy = new float[outDim];
   float *attrValueCpy = new float[attrDim];
   
   float* ptrLinkCpy;
   float* ptrLink;
   bool equal = true;
   try{
 	  
 	  for(int i = 0; i < cpyMap.nmbOfLinks(); i++){
 		   		  
 		  ptrLink = map.ptrLinkByIndex(i);
 		  ptrLinkCpy = cpyMap.ptrLinkByIndex(i);
 		  
 		  
 		  for(int a = 0; a < inpDim; a++){
 			  inpNodeCpy[a] = ptrLinkCpy[a]; 
 			  inpNode[a] = ptrLink[a]; 
 		  };
 		  for(int b = 0; b < outDim; b++){
 			  outNodeCpy[b] = ptrLinkCpy[inpDim + b];
 			  outNode[b] = ptrLink[inpDim + b];
 		  };
 		  for(int c = 0; c < attrDim; c++){
 			  attrValueCpy[c] = ptrLinkCpy[inpDim + outDim + c];
 			  attrValue[c] = ptrLink[inpDim + outDim + c];
 		  };
 		  
 		  cout.flush();

 		  
 		  for(int k = 0; k < inpDim; k++){
 			if(inpNodeCpy[k] != inpNode[k]){
				  cout << "ptrLinkCpy[" << k << 
				  	"]="<< ptrLinkCpy[k] <<
				  	" != inpNode[" << k << "]=" << inpNode[k]<< " \n"; 
 				  equal = false;
 				  break;
 			};
 		  };
 		  
 		  
 		  if(equal){
 			  for(int l = 0; l < outDim; l++){
 				 if(outNodeCpy[l] != outNode[l]){ 					  
				  cout << "ptrLinkCpy[" << l << 
				  	"]="<< outNodeCpy[l] <<
				  	" != outNode[" << l << "]=" << outNode[l]<< " \n"; 
 					  equal = false;
 					  break;
 				  }
 			  }
 		  };  
 		  
 		  if(equal){
 			  for(int m = 0; m < attrDim; m++){
 				 if(attrValueCpy[m] != attrValue[m]){ 					  
				  cout << "ptrLinkCpy[" << m << 
				  	"]="<< attrValueCpy[m] <<
				  	" != attrValue[" << m << "]=" << attrValue[m]<< " \n"; 
 					  equal = false;
 					  break;
 				  }
 			  }
 		  };  
 		  
 		  if(!equal) break;
 	  };
 	  
 	  if(equal){
	    cout << " OK.\n";
 	  }else{
	   	cout << " FAILED. not equal\n" ; // wrong exception
 	  };
   }catch(...){
   	cout << " FAILED. exception.\n" ; // wrong exception
     isFailed = true;
   };
   //////////////////////////////////////////////////
   
   
  
   
   
   /** next test */
    i++; // 
    cout << "\t subtest " << i << "\t";
    ///////////////////////////////////////////////////  
   map.deinit();
   nmbNodes = 100;
   inpDim = 6;
   outDim = 3;
   attrDim = 2;
   map.init(inpDim,outDim,attrDim,1500);
   
   for(int i = 0; i < nmbNodes; i++){
 	  
 	  for(int k = 0; k < inpDim; k++){
 		  inpNode[k] = ((float)rand()) / ((float)RAND_MAX);
 	  };
 	  for(int l = 0; l < outDim; l++){
 			  outNode[l] = ((float)rand()) / ((float)RAND_MAX);
 	  };
 	  map.addLink(inpNode,outNode);
   };

   // create random link
  for(int k = 0; k < inpDim; k++){
	  inpNode[k] = (float)((int)((((float)rand()) / ((float)RAND_MAX))));
  };
  for(int l = 0; l < outDim; l++){
	  outNode[l] = (float)((int)((((float)rand()) / ((float)RAND_MAX))));
  };
  
  equal = true;
   try{
	   int *indexListOut;
	   int *indexListInp;
	   int oldNmbOut = 0;
	   int newNmbOut = 0;
	   int oldNmbInp = 0;
	   int newNmbInp = 0;
	   int c =0 ;
	   for(float d = 0.0; d < sqrt(inpDim); d = d + 0.1){
		   indexListOut = map.getOutputNodeIndicesWithinRegion(outNode,d);
		   indexListInp = map.getInputNodeIndicesWithinRegion(inpNode,d);
		   if(indexListOut != NULL){
			   newNmbOut = indexListOut[0];
			   if(newNmbOut < oldNmbOut){
				   equal = false;
				   break;
			   };
			   oldNmbOut = newNmbOut;
			   delete indexListOut;
		   };
		   if(indexListInp != NULL){
			   newNmbInp = indexListInp[0];
			   if(newNmbInp < oldNmbInp){
				   equal = false;
				   break;
			   };
			   oldNmbInp = newNmbInp;
			   delete indexListInp;
		   };
	   };

	   
 	  	if(equal){
 	  		cout << " OK.\n";
 	  	}else{
 	  		cout << " FAILED. not equal\n" ; // wrong exception
 	  	};
   	}catch(...){
   		cout << " FAILED. exception.\n" ; // wrong exception
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
