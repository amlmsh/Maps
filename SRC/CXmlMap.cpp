#include "CXmlMap.h"
#include <iostream>
#include <stdexcept>


CXmlMap::CXmlMap() : CBaseMap(){
	parser_ = NULL;
	mapHandler_ = NULL;
	
	try {
		XMLPlatformUtils::Initialize();
	}catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		cout << "Error during initialization! :\n";
		cout << "Exception message is: \n"
		<< message << "\n";
		XMLString::release(&message);
		return;
	};


	parser_ = XMLReaderFactory::createXMLReader();
	parser_->setFeature(XMLUni::fgSAX2CoreValidation, true);   
	parser_->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional
	
	mapHandler_ = new CMapSAX2Handler();
	parser_->setContentHandler(mapHandler_);
	parser_->setErrorHandler(mapHandler_);
	
	mapHandler_->init(this);
};


CXmlMap::~CXmlMap(){
	if(parser_ != NULL){
		delete parser_;
	};
	
	if(mapHandler_ != NULL){
		delete mapHandler_;
	};
};

void CXmlMap::readXmlStructure(char *filename) throw (IMapException){
	if((parser_ == NULL) || (mapHandler_ == NULL)){
		string msg = string("Handler and XML reader not initalized yet. Cannot read xml-file.\n");
		throw IMapException(msg,-1);	
	};
	
	// deinitialize the current state of the map
	deinit();
	
	try {
			parser_->parse(filename);
	}catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		string msg = string("Exception message is: \n") + 
			string(message) + string("\n");
			XMLString::release(&message);
		throw IMapException(msg,-1);
	}catch (const SAXParseException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		string msg = string("Exception message is: \n") + 
			string(message) + string("\n");
		XMLString::release(&message);
		throw IMapException(msg,-1);
	}catch(runtime_error &e){
		string msg = string("check structure and semantics of values in xml-file: ") +
			string(filename)  + string("\n");
		throw IMapException(msg,-1);
	}catch (...) {
		string msg = string("Unexpected Exception \n");
		throw IMapException(msg,-1);
	};
};

void CXmlMap::readXmlFileWithoutAttributes(char *filename) throw (IMapException){
	mapHandler_->readLinkAttributes(false);
	try{
		readXmlStructure(filename);
	}catch(IMapException e){
		throw e;
	}catch(...){
		string msg = string("Unexpected Exception \n");
		throw IMapException(msg,-1);		
	};
	return;
};

void CXmlMap::readXmlFile(char *filename) throw (IMapException){
	mapHandler_->readLinkAttributes(true);
	try{
		readXmlStructure(filename);
	}catch(IMapException e){
		throw e;
	}catch(...){
		string msg = string("Unexpected Exception \n");
		throw IMapException(msg,-1);		
	};
	return;
};

void CXmlMap::writeXmlFile(char *filename) throw(IMapException){
	// open file
	ofstream *xmlOutStream;
	try{
		xmlOutStream = new ofstream();
	}catch(bad_alloc e){
		return;
	}catch(...){
		return;
	};
	
	xmlOutStream->open(filename);
	if (!xmlOutStream->is_open()){
		string msg = string("Unable to open xml file: ") +
			string(filename);
		throw (IMapException(msg,-1));
	};

	// write DTD
	outDTD(xmlOutStream);
	
	// write data
	outMap(xmlOutStream);
	
	//close file;
	xmlOutStream->close();
	
	delete xmlOutStream;
	xmlOutStream = NULL;
}

void CXmlMap::outDTD(ofstream *fout){
	*fout << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
	*fout << "<!DOCTYPE Map [\n";
	*fout << "<!ELEMENT Map (Comment?,Link*)>\n";
	*fout << "<!ATTLIST Map\n";
	*fout << "          InpDim       CDATA #REQUIRED\n";
	*fout << "          OutDim       CDATA #REQUIRED\n";
	*fout << "          LinkAttrDim  CDATA #REQUIRED>\n";
	*fout << "<!ELEMENT Comment (#PCDATA)>\n";
	*fout << "<!ELEMENT Link (InputNode+,OutputNode+,LinkAttr+)>\n";
	
	*fout << "<!ELEMENT InputNode (Component+)>\n";
	*fout << "<!ATTLIST InputNode\n";
	*fout << "    Dim         CDATA #REQUIRED>\n";
	
	*fout << "<!ELEMENT OutputNode (Component+)>\n";    
	*fout << "<!ATTLIST OutputNode\n";
	*fout << "    Dim         CDATA #REQUIRED>\n";
	
	*fout << "<!ELEMENT LinkAttr (LinkAttrValue+)>\n";
	*fout << "<!ATTLIST LinkAttr\n";
	*fout << "    Dim         CDATA #REQUIRED>\n";

	*fout << "<!ELEMENT Component EMPTY>\n";    
	*fout << "<!ATTLIST Component\n";
	*fout << "    Value         CDATA #REQUIRED\n";
	*fout << "    NodeIdx       CDATA #REQUIRED>\n";
	
	*fout << "<!ELEMENT LinkAttrValue EMPTY>\n";    
	*fout << "<!ATTLIST LinkAttrValue\n";
	*fout << "    LinkValue     CDATA #REQUIRED\n";
	*fout << "    Idx           CDATA #REQUIRED>\n";
	
	*fout << "]>\n";
};

void CXmlMap::outMap(ofstream *fout){
	*fout << "<Map InpDim=\"" << inpDim_ << "\"";
	*fout <<     " OutDim=\"" << outDim_  << "\"";
	*fout <<     " LinkAttrDim=\"" << attrDim_  << "\">\n";
	
	outComment(fout);
	
	for (int i = 0; i < (indexLastLink_+1); i++){
		outLink(fout,i);
	};
	
	*fout << "</Map>\n";
}


void CXmlMap::getLinkByIdx(float *linkValues, int idx){
	if((idx < 0) || (idx > indexLastLink_)){
		return;
	};
	
	int baseIdx = (inpDim_ + outDim_ + attrDim_)*idx;
	for(int i = 0; i < (inpDim_ + outDim_ + attrDim_); i++){
		linkValues[i] = listOfLinks_[baseIdx + i];
	}
}

void CXmlMap::outInpNode(ofstream *fout, float *values, int dim){
	*fout << "\t\t<InputNode Dim=\"" << dim <<    "\">\n";

	for(int i = 0; i < dim; i++){
		*fout << "\t\t\t<Component Value=\"" <<
			values[i] << "\" NodeIdx=\"" << i << "\" />\n";
	};
	
	*fout << "\t\t</InputNode>\n";	;
}

void CXmlMap::outOutNode(ofstream *fout, float *values, int dim){
	*fout << "\t\t<OutputNode Dim=\"" << dim <<    "\">\n";

	for(int i = 0; i < dim; i++){
		*fout << "\t\t\t<Component Value=\"" <<
			values[i] << "\" NodeIdx=\"" << i << "\" />\n";
	};
	
	*fout << "\t\t</OutputNode>\n";	
}

void CXmlMap::outAttrValues(ofstream *fout, float *values, int dim){
	*fout << "\t\t<LinkAttr Dim=\"" << dim <<    "\">\n";

	for(int i = 0; i < dim; i++){
		*fout << "\t\t\t<LinkAttrValue LinkValue=\"" <<
			values[i] << "\" Idx=\"" << i << "\" />\n";
	};
	
	*fout << "\t\t</LinkAttr>\n";	
}

void CXmlMap::outLink(ofstream *fout, int idx){
	*fout << "\t<Link>\n";

	float linkValues[inpDim_ + outDim_ + attrDim_];
	getLinkByIdx(linkValues, idx);
	
	outInpNode(fout, linkValues, inpDim_);
	outOutNode(fout, &(linkValues[inpDim_]), outDim_);
	outAttrValues(fout,&(linkValues[inpDim_ + outDim_]),attrDim_);
	
	*fout << "\t</Link>\n";	;
}

void CXmlMap::outComment(ofstream *fout){
	*fout << "\t<Comment>\n";
	*fout << comments_ << endl;
	*fout << "\t</Comment>\n";	;	;
}

void CXmlMap::writeComments(string s){
	comments_ = string(s);
};

string CXmlMap::getComments(){
	return (comments_);
};


/*
 * 
 * 
 * Handler implementation
 * 
 * 
 * 
 */


CMapSAX2Handler::CMapSAX2Handler(){
   	xmlMap_ = XMLString::transcode("Map");
   	xmlMapInpDimAttr_ = XMLString::transcode("InpDim");
   	xmlMapOutDimAttr_ = XMLString::transcode("OutDim");
   	xmlMapLinkAttrDimAttr_ = XMLString::transcode("LinkAttrDim");
   	xmlLink_ = XMLString::transcode("Link");
   	xmlInpNode_ = XMLString::transcode("InputNode");
   	xmlOutNode_ = XMLString::transcode("OutputNode");
   	xmlComponent_ = XMLString::transcode("Component");
   	xmlCompAttrValue_ = XMLString::transcode("Value");
   	xmlCompAttrIdx_ = XMLString::transcode("NodeIdx");
   	
	xmlLinkAttr_ = XMLString::transcode("LinkAttr");
	xmlLinkAttrComponent_ = XMLString::transcode("LinkAttrValue");
	xmlLinkAttrCompValue_ = XMLString::transcode("LinkValue");
	xmlLinkAttrCompIdx_ = XMLString::transcode("Idx");

	
   	readInpNode_ = false;
   	readOutNode_ = false;
   	readNodeAttr_ = false;
   	firstLink_ = true;	;;
}
CMapSAX2Handler::~CMapSAX2Handler(){
	;
}

void CMapSAX2Handler::readLinkAttributes(bool b){
	readLinkAttributes_ = b;
}
void CMapSAX2Handler::startElement(const   XMLCh* const    uri,const   XMLCh* const    localname, const   XMLCh* const    qname, const   Attributes&     attrs){  
	    if(!XMLString::compareString(XMLString::transcode(qname),XMLString::transcode(xmlMap_))){
	    	char * chInpDim  = XMLString::transcode(attrs.getValue(xmlMapInpDimAttr_));
	    	char * chOutDim  = XMLString::transcode(attrs.getValue(xmlMapOutDimAttr_));
	    	inpDim_ = atoi(chInpDim);
	    	outDim_ = atoi(chOutDim);



	    	if(readLinkAttributes_){
	    		char * chNodeAttrDim = XMLString::transcode(attrs.getValue(xmlMapLinkAttrDimAttr_));
	    		attrDim_ = atoi(chNodeAttrDim);
	    	}else{
	    		char * chNodeAttrDim = XMLString::transcode(attrs.getValue(xmlMapLinkAttrDimAttr_));
	    		attrDim_ = 1;
	    	}
	    	
	    	// chech boundaries
	    	if((inpDim_ < 1) || (outDim_ < 1) || (attrDim_ < 1)){
	    		throw (runtime_error("Map and attribute dimensions in xml file less than 1.\n"));
	    	}   	    	
	    	// create storage
	    	inpNodeValues_ = new float[inpDim_];
	    	outNodeValues_ = new float[outDim_];
	    	nodeAttrValues_ = new float[attrDim_];
	    	
	    	map_->init(inpDim_,outDim_,attrDim_,1000);
	    };
	    

	    if(!XMLString::compareString(XMLString::transcode(qname),XMLString::transcode(xmlInpNode_))){
	    	inpNodeReadMode_ = true;
	    	readInpNode_ = true;
	    };
	    
	    if(!XMLString::compareString(XMLString::transcode(qname),XMLString::transcode(xmlOutNode_))){	    	
	    	inpNodeReadMode_ = false;
	    	readOutNode_  = true;
	    };
	    
	    if(!XMLString::compareString(XMLString::transcode(qname),XMLString::transcode(xmlLinkAttr_))){
	    	if(readLinkAttributes_){
	    		inpNodeReadMode_ = false;
	    		outNodeReadMode_  = false;
	    		readNodeAttr_ = true;
	    	};
	    };
	    
	    if(!XMLString::compareString(XMLString::transcode(qname),XMLString::transcode(xmlComponent_))){
	    	int index;
	    	float value;
	    	// get attribute value
	    	char * chValue = XMLString::transcode(attrs.getValue(xmlCompAttrValue_));
	    	char * chIdx = XMLString::transcode(attrs.getValue(xmlCompAttrIdx_));
	    	index = atoi(chIdx);
	    	value = atof(chValue);
	    	
	    	if(inpNodeReadMode_){
	    		if(index >= inpDim_){
	    			throw (runtime_error("wrong index in xml file.\n"));
	    		};
	    		inpNodeValues_[index] = value;
	    	}else{
	    		if(index >= outDim_){
	    			throw (runtime_error("wrong index in xml file.\n"));
	    		};
	    		outNodeValues_[index] = value;
	    	};
	    	
	    };
	    
	    
	    if(!XMLString::compareString(XMLString::transcode(qname),XMLString::transcode(xmlLinkAttrComponent_)) ){

	    	int index;
	    	float value;
	    	// get attribute value
	    	char * chValue = XMLString::transcode(attrs.getValue(xmlLinkAttrCompValue_));
	    	char * chIdx = XMLString::transcode(attrs.getValue(xmlLinkAttrCompIdx_));
	    	
	    	if(readLinkAttributes_){
	    		index = atoi(chIdx);
	    		value = atof(chValue);
	    	}else{
	    		index = 0;
	    		value = 0.0;
	    	}
	    	
	    	
	    	if(index >= attrDim_){
	    		throw (runtime_error("wrong index in xml file.\n"));
	    	};
	    	nodeAttrValues_[index] = value;
	    };
};


void CMapSAX2Handler::endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname){
	
    if(!XMLString::compareString(XMLString::transcode(qname),XMLString::transcode(xmlLink_))){
    	
    	if(readLinkAttributes_){
    		if(readOutNode_ && readInpNode_ && readNodeAttr_){
    			// put data into map    		 
    			map_->addLink(inpNodeValues_,outNodeValues_,nodeAttrValues_);
    		
    			// ready for new data
    			readOutNode_ = false;
    			readInpNode_ = false;
    			readNodeAttr_ = false;
    		}else{
    			throw (runtime_error(" New link detected but haven't read the old link completely.\n"));
    		};
    	}else{
    		if(readOutNode_ && readInpNode_){
    			// put data into map
    			float f = 0.0;
    			map_->addLink(inpNodeValues_,outNodeValues_,&f);
    		
    			// ready for new data
    			readOutNode_ = false;
    			readInpNode_ = false;
    			readNodeAttr_ = false;
    		}else{
    			throw (runtime_error(" New link detected but haven't read the old link completely.\n"));
    		};
    		
    	};
    };
};

void CMapSAX2Handler::fatalError(const SAXParseException& exception){
	string msg = string("Fatal Error: ") + 
		string(XMLString::transcode(exception.getMessage())) + 
		string("\n");
	throw (IMapException(msg,-1));
};

void  CMapSAX2Handler::init(CXmlMap *map){
	map_ = map;
	readLinkAttributes_ = true;
};

