#ifndef UWA_DCS_REVERB_SW_MAPS_CBASEMAP_H_
#define UWA_DCS_REVERB_SW_MAPS_CBASEMAP_H_

#include <string>
using namespace std;

/**
 * 
 * \mainpage Reverb SW_BASEMAP Documentation
 *
 * \author Martin Huelse
 * 
 *
 * University of Wales, Aberystwyth <BR>
 * Dept. Computer Science
 *
 * email: msh@aber.ac.uk
 * 
 * \date 15.05.2008
 *
 * \section intro_sec Introduction
 * These implementation provides the tools 
 * for the generation of sensorymotor maps
 * between spaces of arbitrary dimensions.
 * 
 *  
 */
 
 
/** 
 *
 * \file CBaseMap.h 
 *
 * \brief Contains the definition of a class
 * implementing a mapping between spaces of 
 * arbitrary dimension.
 */


/**
 * 
 * \class IMapException 
 * \brief Expection for our \ref CBaseMap
 * implementation and is derived classes.
 * 
 */
class IMapException {
	public:
	IMapException();

	/**
	 * 
	 * Constructor
	 * @param s const char*, string contains messages that 
	 * might help to give proper error messages, because 
	 * the are given while throwing the exception.
	 * @param c int, might used for specific codes
	 */
	IMapException(string s, int c);
	~IMapException();
	/**
	 * 
	 * \return string data about current error state
	 */
	string getMsg();
	/**
	 * 
	 * \return string data about current error state incl. additional information
	 */
	string getSupport();
	/**
	 * contains error code provided by underlying libraries
	 */
	int getCode();
	
	protected:
	/**
	 * 
	 * might contain specific data with respect to
	 * the error
	 */ 
	string msg_;
	/**
	 * 
	 * might contain general information for 
	 * the users about the reasons of the error
	 */ 
	string support_;
	/**
	 * 
	 * might contain a specific error code
	 */ 
	int code_;
};

/**
 * 
 *  \class CBaseMap
 *  \brief     A class implementing the essential
 *   functionality for sensorymotor mappings between
 *   spaces of arbitrary dimension.
 *
 * 
 *  \author Martin Huelse, msh@aber.ac.uk
 *  
 *  \date    15.05.2008
 *
 */
class CBaseMap {

	public:
		/**
		 * 
		 * Constructor
		 */
		CBaseMap();
		/**
		 * 
		 * Destructor
		 * 
		 */
		~CBaseMap();
		
		/**
		 * 
		 * \fn void init(int inpDim, int outDim, int attrDim, int initSize) throw (IMapException)
		 * 
		 * Initialization of the map. 
		 * @param inpDim int dimension of the input nodes
		 * @param outDim int dimension of the output node
		 * @param attrDim int dimension of the link attributes
		 * @param initSize int storage size of the map after
		 * initialization. If the map is full storage will be 
		 * enlarged in such a way that initSize additional 
		 * links can be stored. 
		 */
		void init(int inpDim, int outDim, int attrDim, int initSize) throw (IMapException);
		
		/**
		 * 
		 * Deinitialization. The map incl. its parameter
		 * about the dimension of input and output space
		 * is completely deleted. Allocated memory is free. 
		 */
		void deinit();
		
		/**
		 * 
		 * Deletes all nodes in the map. The parameters
		 * and the storage size remain the same.
		 */
		void clean();
		
		/**
		 * 
		 * 
		 * Adds a link to the map. If map is full new 
		 * memory will be allocated. If this fails an
		 * exception is thrown. A link is defined by 
		 * an input node and an output node. Each 
		 * link has attribute values. Here there are set to 
		 * zero per default. The nodes
		 * are represented by 1-dim. arrays of floats.
		 * The size of the array must not be smaller then 
		 * the defined input and output dimension. 
		 * @param inpNode *float  1-dim array of floats defining
		 * the input node. The size of the array must be at 
		 * least \ref inpDim_ .
		 * @param outNode *float  1-dim array of floats defining
		 * the output node. The size of the array must be 
		 * at least \ref outDim_ .
		 * 
		 * \anchor addLinkWithoutAttributes
		 */
		void addLink(float *inpNode, float *outNode) throw (IMapException);
		
		/**
		 * As \ref addLinkWithoutAttributes but here the
		 * attribute values are explicitly given.
		 * @param inpNode *float  1-dim array of floats defining
		 * the input node. The size of the array must be at 
		 * least \ref inpDim_ .
		 * @param outNode *float  1-dim array of floats defining
		 * the output node. The size of the array must be 
		 * at least \ref outDim_ .
		 * @param linkAttributes *floar 1-dim array of floats
		 * defining the attributes of the link. The size of the array
		 * must be ar leasr \ref attrDim_ .
		 * 
		 */
		void addLink(float *inpNode, float *outNode, float *linkAttributes) throw (IMapException);
		
		/**
		 * 
		 * returns the number of currently stored 
		 * links in the map.
		 * \return int > 0
		 */
		int  nmbOfLinks();  
		
		
		/***
		 * 
		 * Remove node of the given index. 
		 * After this method call the index 
		 * organization is totally different. 
		 * Any former index value might refer to a total
		 * different link.
		 * @param nodeIdx int  index of the node in this 
		 * mapping.
		 * If given index doesn't exist nothing changes.
		 */
		void removeLink(int nodeIdx);
		
		/**
		 * 
		 * Returns a list of integers storing the index values
		 * of all output nodes which distance from the given 
		 * output node is smaller than value of radius.
		 * @param outNodeValues *float 1-dim array defining
		 * the output node which the defines the centre of the region.
		 * @param radius float maximal distance defining the 
		 * region for our search of nodes
		 * \return *int list l of indexes of nodes within the
		 * defined region. The first entry l[0] is the number of 
		 * nodes found in this region.
		 */
		int *getOutputNodeIndicesWithinRegion(float *outNodeValues, float radius);
		
		
		/**
		 * 
		 * Returns a list of integers storing the index values
		 * of all input nodes which distance from the given 
		 * output node is smaller or equal than value of radius.
		 * @param inpNodeValues *float 1-dim array, which defines
		 * the input node which is the centre of the region.
		 * @param radius float maximal distance defining the 
		 * region for our search of nodes
		 * \return *int list l of indexes of nodes within the
		 * defined region. The first entry l[0] is the number of 
		 * nodes found in this region.
		 */
		int *getInputNodeIndicesWithinRegion(float *inpNodeValues, float radius);
		
		
		/**
		 * 
		 * returns the link, defined by input and output node, which
		 * is the closest to the given point in the output space of the map.  
		 * @param outNodeValues *float 1-dim array defining a 
		 * point in the output space. Array dimension must not less then \ref outDim_
		 * @param inpNodeStorage *float  1-dim array large enough to
		 * store all values defining a input node, dimension must at 
		 * least \ref inpDim_ . 
		 * @param outNodeStorage *float 1-dim array large enough to 
		 * store all values defining a output node of this map. Dimension
		 * must be at least \ref outDim_ .
		 * The last two parameters define the link in the map,
		 * which output node is closest to the point in the output space
		 * given by the first parameter.
		 */
		void getClosestOutputNodeLink(float* outNodeValues, float* inpNodeStorage, float* outNodeStorage) throw (IMapException);
		
		
		
		/**
		 * 
		 * returns the link, defined by input and output node, which
		 * is the closest to the given point in the input space of the map.  
		 * @param inpNodeValues *float 1-dim array defining a 
		 * point in the input space of the map. Array dimension must not less then \ref outDim_
		 * @param inpNodeStorage *float  1-dim array large enough to
		 * store all values defining a input node, dimension must at 
		 * least \ref inpDim_ . 
		 * @param outNodeStorage *float 1-dim array large enough to 
		 * store all values defining a output node of this map. Dimension
		 * must be at least \ref outDim_ .
		 * The last two parameters define the link in the map,
		 * which input node is closest to the point in the input space
		 * given by the first parameter.
		 */
		void getClosestInputNodeLink (float* inpNodeValues, float* inpNodeStorage, float* outNodeStorage) throw (IMapException);
		
		/**
		 * 
		 * Calculates the output node of a given 
		 * input node in the map. If the is no such 
		 * input node then an exception is thrown.  
		 * @param inpNode *float 1-dim array containing the 
		 * values of an input node in the map.
		 * @param outNodeStorage *float storage of the 
		 * corresponding output node.
		 * 
		 * There can be more than one link in the map
		 * which start with the given input node. The
		 * function returns the output node values of 
		 * the link which is found first. 
		 */
		void getOutputNode(float* inpNode, float* outNodeStorage) throw (IMapException);

		/**
		 * 
		 * Calculates the input node of a given 
		 * link in the map defined by the given output node. 
		 * If the is no such 
		 * output node then an exception is thrown.  
		 * @param outNode *float 1-dim array containing the 
		 * values of an output node in the map.
		 * @param inpNodeStorage *float storage of the 
		 * corresponding input node.
		 * 
		 * There can be more than one link in the map
		 * which end with the given output node. The
		 * function returns the input node values of 
		 * the link which is found first. 
		 */
		void getInputNode (float* outNode, float* inpNodeStorage) throw (IMapException);
		
		
		/**
		 * 
		 * return a pointer pointing to the 
		 * i-th link. If i is out of range, the return 
		 * value is the null-pointer NULL.
		 * @param i int  number of the link the list,
		 * starting with 0
		 */
		float *ptrLinkByIndex(int i);
		
		
		/**
		 * 
		 * Returns the dimension of the 
		 * input space of the map.
		 * \return int 0 iff map is not initialized yet
		 * otherwise it is a positive number.
		 */
		int getInpDim();
		
		/**
		 * 
		 * Returns the dimension of the 
		 * input space of the map.
		 * \return int 0 iff map is not initialized yet
		 * otherise it is a positive number. 
		 */
		int getOutDim();

		/**
		 * 
		 * Returns the dimension of the 
		 * attribute values of the link in this mapping.
		 * \return int -1 iff map is not initialized yet
		 * otherise it is a positive number. 
		 */
		int getAttrDim();

		
	protected:
		
		void reCopy(float *destination, float *source, int nmb);
		float euclidDist(float *inp1, float *inp2, int nmb);
		
		
		int initSize_;
		int maxNmbLinks_;
		int indexLastLink_;
		
		/**
		 * 
		 * \var inpDim_
		 * 
		 *	Dimension of the input space.
		 * This value is set during initialization
		 * of the map \ref  init(int inpDim, int outDim, int initSize) throw (IMapException)
		 */
		int inpDim_;
		
		/**
		 * 
		 * \var outDim_
		 * 
		 *	Dimension of the output space.
		 * This value is set during initialization
		 * of the map \ref init(int inpDim, int outDim, int initSize) throw (IMapException)
		 */
		int outDim_;
		
		/**
		 * 
		 *  \var attrDim_
		 * 
		 *  Dimension of the attribute values. This values
		 * is set during initialisation of the mapping 
		 * \ref  init(int inpDim, int outDim, int initSize) throw (IMapException)
		 */
		int attrDim_;
		
		float *listOfLinks_;		
};

#endif /*UWA_DCS_REVERB_SW_MAPS_CBASEMAP_H_*/
