#ifndef  UWA_DCS_REVERB_SW_MAPS_CXMLMAP_H_
#define  UWA_DCS_REVERB_SW_MAPS_CXMLMAP_H_

#include "CBaseMap.h"
#include <iostream>
#include <fstream>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
XERCES_CPP_NAMESPACE_USE

/** 
 *
 * \file CXmlMap.h 
 *
 * \brief Contains the definition of a class
 * implementing a mapping between spaces of 
 * arbitrary dimension.
 */



class CMapSAX2Handler;



/**
 * 
 *  \class CXmlMap
 *  \brief     A class implementing the essential
 *   functionality for sensorymotor mappings between
 *   spaces of arbitrary dimension.
 *
 * 
 *  \author Martin Huelse, msh@aber.ac.uk
 *  
 *  \date    16.05.2008
 *
 */
class CXmlMap : public CBaseMap {

	public: 
		CXmlMap();
		~CXmlMap();
		
		void readXmlFile(char *filename) throw (IMapException);	
		void readXmlFileWithoutAttributes(char *filename) throw (IMapException);
		void writeXmlFile(char *filename) throw(IMapException);
		void writeComments(string s);
		string getComments();

		
	protected:
		void readXmlStructure(char *filename) throw (IMapException);	
		void getLinkByIdx(float *linkValues, int i);			
		void outInpNode(ofstream *fout, float *values, int dim);
		void outOutNode(ofstream *fout, float *values, int dim);
		void outAttrValues(ofstream *fout, float *values, int dim);
		void outLink(ofstream *fout, int idx);
		void outComment(ofstream *fout);
		void outDTD(ofstream *fout);
		void outMap(ofstream *fout);
		
		SAX2XMLReader* parser_;
		CMapSAX2Handler* mapHandler_;
		
		
		string comments_;
};

/**
 * 
 *  \class CMapSAX2Handler
 *  \brief     A class implementing
 * 
 *  \author Martin Huelse, msh@aber.ac.uk
 *  
 *  \date    16.05.2008
 *
 */
class CMapSAX2Handler : public DefaultHandler {
public:
	CMapSAX2Handler();
	~CMapSAX2Handler();
    void startElement(const   XMLCh* const    uri,const   XMLCh* const    localname, const   XMLCh* const    qname, const   Attributes&     attrs);
    void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
    void fatalError(const SAXParseException& exception);
    void init(CXmlMap *map);
    void readLinkAttributes(bool b);
    
protected:
	

	
	XMLCh* xmlMap_;
	XMLCh* xmlMapInpDimAttr_;
	XMLCh* xmlMapOutDimAttr_;
	XMLCh* xmlMapLinkAttrDimAttr_;
	XMLCh* xmlLink_;
	XMLCh* xmlInpNode_;
	XMLCh* xmlOutNode_;
	XMLCh* xmlNodeAttr_;
	
	XMLCh* xmlComponent_;
	XMLCh* xmlCompAttrValue_;
	XMLCh* xmlCompAttrIdx_;
	
	XMLCh* xmlLinkAttr_;
	XMLCh* xmlLinkAttrComponent_;
	XMLCh* xmlLinkAttrCompValue_;
	XMLCh* xmlLinkAttrCompIdx_;
	
	float *inpNodeValues_;
	float *outNodeValues_;
	float *nodeAttrValues_;
	
	int inpDim_;
	int outDim_;
	int attrDim_;
	
	bool readInpNode_;
	bool readOutNode_;
	bool readNodeAttr_;
	
	bool inpNodeReadMode_;
	bool outNodeReadMode_;
	bool nodeAttrReadMode_;
	
	bool firstLink_;
	
	
	bool readLinkAttributes_;
	
	CXmlMap *map_;
};


#endif /* UWA_DCS_REVERB_SW_MAPS_CXMLMAP_H_*/
