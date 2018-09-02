#ifndef UWA_DCS_REVERB_SW_MAPS_CMEMORYMAP_H_
#define UWA_DCS_REVERB_SW_MAPS_CMEMORYMAP_H_


#include "CBaseMap.h"
#include <iostream>
#include <fstream>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
XERCES_CPP_NAMESPACE_USE

/** 
 *
 * \file CMemoryMap.h 
 *
 * \brief Contains the definition of a class
 * implementing a process of activity calculation
 * based on maps \ref CBaseMap .
 */



/**
 * 
 *  \class CMemoryMap
 *  \brief     A class implementing a 
 * process of activity calculation based on 
 * a map \ref CBaseMap
 *
 * 
 *  \author Martin Huelse, msh@aber.ac.uk
 *  
 *  \date    21.05.2008
 *
 */
class CMemoryMap {

public:
	CMemoryMap();
	~CMemoryMap();
	
	/**
	 * 
	 * Initialize the memory with a map.
	 * The map must be not empty and 
	 * the size of input and output space dimension
	 * must be similar. Otherwise a Exception is 
	 * thrown.
	 * @param map *CBaseMap initialized and containing
	 * links 
	 */
	void initMap(CBaseMap *map) throw (IMapException);
	
	/**
	 * 
	 * Deinit the memory. The map is deleted as well.
	 */
	void deinitMap();
	
	/**
	 * 
	 *  Returns the dimension of the memory derived
	 * from the map the memory map is initialized.
	 * \return 0 iff memory map is not initialized yet.
	 * Otherwise a positive number inidicating the 
	 * dimension of the memory map.
	 */
	int getDim();

	/**
	 * 
	 * returns the number of links in the map.
	 * This defines the number of maximal input 
	 * or output nodes in the given map.
	 * \return nmb of links > 0. If return values
	 * is zero or less then the map is not 
	 * initialized yet.
	 */
	int getNmbLinks();
	
	/**
	 * 
	 * Takes an input activation pattern defined by the
	 * iput nodes and calculates the activation of the 
	 * output pattern derived by the links in the map.
	 * @param inputNodes *float  1-dim array storing 
	 * nmbInputNodes many input nodes. 
	 * @param nmbInputNodes int > 0, defines the number of 
	 * nodes stored in the *inputNodes array.
	 * Excpetion is thrown if memory map is not initilized
	 * or if nmbInputNodes is less than 1.
	 */
	void updateActivation(int nmbInputNodes, float *inputNodes) throw (IMapException);
	
	/**
	 * 
	 * Take an input activation pattern defined by
	 * the input nodes, replaces these nodes with their 
	 * clostest node, if not existing in the input layer,
	 * and calculates the activation of the output pattern
	 * determined by the given map.
 	 * @param inputNodes *float  1-dim array storing 
	 * nmbInputNodes many input nodes. 
	 * @param nmbInputNodes int > 0, defines the number of 
	 * nodes stored in the inputNodes array.
	 * Excpetion is thrown if memory map is not initilized
	 * or if nmbInputNodes is less than 1.
	 * 
	 */
	void updateActivationWithClosestNodes(int nmbInputNodes, float *inputNodes) throw (IMapException);
	
	/**
	 * 
	 * returns number of output nodes with activations 
	 * larger zero. Those nodes are stored in 
	 * the array outputNodes. The actual activation
	 * values are given by the array of unsigned int
	 * activationValues.
	 * 
	 */
	float *getActiveNodes(int *nmbOutputNodes, unsigned int* activationValues) throw (IMapException);
	

	/**
	 * 
	 * get activations values. The 1-dim array 
	 * must have the size n, where n is the number
	 * of links stored in the map. 
	 * 
	 */
	void getAllActivationsValues(unsigned int* valuesStorage)throw (IMapException); 
	
	/**
	 * 
	 * \return sum long int sum of all activation values.
	 */
	long int sumActivation();
	
protected:
	
	CBaseMap *map_;

	/**
	 * Activations values
	 */
	unsigned int *activations_;
	
	/**
	 * 
	 */
	int nmbLinks_;
	
	int mapDim_;
};

#endif /*UWA_DCS_REVERB_SW_MAPS_CMEMORYMAP_H_*/
