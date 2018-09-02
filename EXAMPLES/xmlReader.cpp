#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include <iostream>
#include <stdexcept>

using namespace std;

XERCES_CPP_NAMESPACE_USE



class MapSAX2Handler : public DefaultHandler {
public:
    void startElement(
        const   XMLCh* const    uri,
        const   XMLCh* const    localname,
        const   XMLCh* const    qname,
        const   Attributes&     attrs
    ){
    	   // transcode() is an user application defined function which
    	    // converts unicode strings to usual 'char *'. Look at
    	    // the sample program SAX2Count for an example implementation.
    	    //cout << "I saw element: " <<  XMLString::transcode(qname) << endl;
    	    
    	    if(!XMLString::compareString(XMLString::transcode(qname),XMLString::transcode(xmlMap_))){
    	    	char * chInpDim = XMLString::transcode(attrs.getValue(xmlMapInpDimAttr_));
    	    	char * chOutDim = XMLString::transcode(attrs.getValue(xmlMapOutDimAttr_));
    	    	inpDim_ = atoi(chInpDim);
    	    	outDim_ = atoi(chOutDim);
    	    	// chech boundaries
    	    	if((inpDim_ < 1) || (outDim_ < 1)){
    	    		throw (runtime_error("Map dimension in xml file less than 1.\n"));
    	    	}   	    	
    	    	// create storage
    	    	inpNodeValues_ = new float[inpDim_];
    	    	outNodeValues_ = new float[outDim_];
    	    };
    	    
    	    if(!XMLString::compareString(XMLString::transcode(qname),XMLString::transcode(xmlLink_))){
    	    	if(firstLink_ || (readOutNode_ && readInpNode_)){
    	    		firstLink_ = false;
    	    		// put data into map
    	    		   	    		
    	    		
    	    		// ready for new data
    	    		readOutNode_ = false;
    	    		readInpNode_ = false;
    	    	}else{
    	    		throw (runtime_error(" New link detected but haven't read the old link completely.\n"));
    	    	};
    	    	
    	    };

    	    if(!XMLString::compareString(XMLString::transcode(qname),XMLString::transcode(xmlInpNode_))){
    	    	inpNodeReadMode_ = true;
    	    	readInpNode_ = true;
    	    };
    	    
    	    if(!XMLString::compareString(XMLString::transcode(qname),XMLString::transcode(xmlOutNode_))){
    	    	inpNodeReadMode_ = false;
    	    	readOutNode_ = true;
    	    };
    	    
    	    if(!XMLString::compareString(XMLString::transcode(qname),XMLString::transcode(xmlComponent_))){   	    	
    	    	// get attribute value
    	    	char * chValue = XMLString::transcode(attrs.getValue(xmlCompAttrValue_));
    	    	char * chIdx = XMLString::transcode(attrs.getValue(xmlCompAttrIdx_));
    	    	int index = atoi(chIdx);
    	    	float value = atof(chValue);
    	    	
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
    	    
    };
    
    
    void fatalError(const SAXParseException& exception){
        cout << "Fatal Error: " << 
        	(XMLString::transcode(exception.getMessage())) << 
        		" at line: " << exception.getLineNumber() << endl;;
    };
    
    void initHandler(){
    	xmlMap_ = XMLString::transcode("Map");
    	xmlMapInpDimAttr_ = XMLString::transcode("InpDim");
    	xmlMapOutDimAttr_ = XMLString::transcode("OutDim");
    	xmlLink_ = XMLString::transcode("Link");
    	xmlInpNode_ = XMLString::transcode("InputNode");
    	xmlOutNode_ = XMLString::transcode("OutputNode");
    	xmlComponent_ = XMLString::transcode("Component");
    	xmlCompAttrValue_ = XMLString::transcode("Value");
    	xmlCompAttrIdx_ = XMLString::transcode("NodeIdx");
    	readInpNode_ = false;
    	readOutNode_ = false;
    	firstLink_ = true;
    };
    
protected:
	XMLCh* xmlMap_;
	XMLCh* xmlMapInpDimAttr_;
	XMLCh* xmlMapOutDimAttr_;
	XMLCh* xmlLink_;
	XMLCh* xmlInpNode_;
	XMLCh* xmlOutNode_;
	XMLCh* xmlComponent_;
	XMLCh* xmlCompAttrValue_;
	XMLCh* xmlCompAttrIdx_;
	
	float *inpNodeValues_;
	float *outNodeValues_;
	int inpDim_;
	int outDim_;
	
	bool readInpNode_;
	bool readOutNode_;
	bool inpNodeReadMode_;
	bool firstLink_;
};



int main (int argc, char* args[]) {

	try {
		XMLPlatformUtils::Initialize();
	}catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		cout << "Error during initialization! :\n";
		cout << "Exception message is: \n"
		<< message << "\n";
		XMLString::release(&message);
		return 1;
	};

	char* xmlFile = (char *)"map.xml";
	SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
	parser->setFeature(XMLUni::fgSAX2CoreValidation, true);   
	parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional
	
	MapSAX2Handler* defaultHandler = new MapSAX2Handler();
	parser->setContentHandler(defaultHandler);
	parser->setErrorHandler(defaultHandler);

	
	defaultHandler->initHandler();
	
	
	try {
		parser->parse(xmlFile);
	}catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		cout << "Exception message is: \n"
		<< message << "\n";
		XMLString::release(&message);
		return -1;
	}catch (const SAXParseException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		cout << "Exception message is: \n"
		<< message << "\n";
		XMLString::release(&message);
		return -1;
	}catch(runtime_error &e){
		cout << "check structure and semantics of values in xml-file: " << xmlFile << endl;
	}catch (...) {
		cout << "Unexpected Exception \n" ;
		return -1;
	}

	delete parser;
	delete defaultHandler;
	return 0;
};



