#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>

#include <iostream>
#include <stdexcept>

#include "CXmlMap.h"

using namespace std;

XERCES_CPP_NAMESPACE_USE

int main (int argc, char* args[]) {
	CXmlMap map;
	char *filename= "map.xml";
	
	map.readXmlFile(filename);
	map.writeComments(string("\t\t copy of: "));
	map.writeComments(map.getComments() + string(filename));
	
	map.writeXmlFile("Copy.xml");
};
