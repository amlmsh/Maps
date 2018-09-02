#include <math.h>
#include <iostream>
#include <cstdlib>
#include "CMemoryMap.h"
#include "CBaseMap.h"
#include "CXmlMap.h"

using namespace std;

int main(int argc, char *argv[]) {

	int i = 0;
	char* unitName = argv[0];
	bool isFailed = false;

	// indicating test start of this unit
	cout << "\nTesting:\t" << unitName << endl;

	CBaseMap map;
	CMemoryMap mem;

	/** next test */
	i++; //
	cout << "\t subtest " << i << "\t";
	///////////////////////////////////////////////////
	if (mem.getNmbLinks() < 1) {
		cout << " OK.\n";
	} else {
		cout << mem.getNmbLinks();
		cout << " FAILED.\n"; // both must be the same
		isFailed = true;
	};
	//////////////////////////////////////////////////

	/** next test */
	i++; //
	cout << "\t subtest " << i << "\t";
	///////////////////////////////////////////////////
	if (mem.getDim() < 1) {
		cout << " OK.\n";
	} else {
		cout << " FAILED.\n"; // both must be the same
		isFailed = true;
	};
	//////////////////////////////////////////////////

	/** next test */
	i++; //
	cout << "\t subtest " << i << "\t";
	CBaseMap* tmpMap = NULL;
	///////////////////////////////////////////////////
	try {
		mem.initMap(tmpMap);
		cout << " FAILED.\n"; // need an exception
		isFailed = true;
	} catch (IMapException e) {
		cout << " OK.\n";
	} catch (...) {
		cout << " FAILED. wrong exception\n"; // both must be the same
		isFailed = true;
	}
	//////////////////////////////////////////////////

	/** next test */
	i++; //
	cout << "\t subtest " << i << "\t";
	CXmlMap* tmpXmlMap = NULL;
	///////////////////////////////////////////////////
	try {
		mem.initMap(tmpXmlMap);
		cout << " FAILED.\n"; // both must be the same
		isFailed = true;
	} catch (IMapException e) {
		cout << " OK.\n"; //<< e.getMsg() << endl;;
	} catch (...) {
		cout << " FAILED. wrong exception\n"; // both must be the same
		isFailed = true;
	}
	//////////////////////////////////////////////////

	/** next test */
	i++; //
	cout << "\t subtest " << i << "\t";
	CXmlMap xmlMap;
	float *link;
	try {

		xmlMap.deinit();
		xmlMap.init(2, 1, 1, 200);

		link = new float[3];
		xmlMap.addLink(link, link);
	} catch (IMapException e) {
		cout << " FAILED. not expected exception\n" << e.getMsg() << endl; // both must be the same
		isFailed = true;
	} catch (...) {
		cout << " FAILED. not expected exception\n"; // both must be the same
		isFailed = true;
	}

	///////////////////////////////////////////////////
	try {
		mem.initMap(&xmlMap);
		cout << " FAILED.\n";
		isFailed = true;
	} catch (IMapException e) {
		cout << " OK.\n"; // << e.getMsg() << endl;
	} catch (...) {
		cout << " FAILED. wrong exception\n"; // both must be the same
		isFailed = true;
	}
	//////////////////////////////////////////////////

	/** next test */
	i++; //
	cout << "\t subtest " << i << "\t";
	try {
		xmlMap.deinit();
		xmlMap.init(1, 2, 1, 200);
		xmlMap.addLink(link, link);
	} catch (IMapException e) {
		cout << " FAILED. not expected exception\n" << e.getMsg() << endl; // both must be the same
		isFailed = true;
	} catch (...) {
		cout << " FAILED. not expected exception\n"; // both must be the same
		isFailed = true;
	}

	///////////////////////////////////////////////////
	try {
		mem.initMap(&xmlMap);
		cout << " FAILED.\n";
		isFailed = true;
	} catch (IMapException e) {
		cout << " OK.\n"; // << e.getMsg() << endl;
	} catch (...) {
		cout << " FAILED. wrong exception\n"; // both must be the same
		isFailed = true;
	}
	//////////////////////////////////////////////////

	/** next test */
	i++; //
	cout << "\t subtest " << i << "\t";
	xmlMap.deinit();
	xmlMap.init(2, 2, 1, 200);
	xmlMap.addLink(link, link);
	///////////////////////////////////////////////////
	try {
		mem.initMap(&xmlMap);
		cout << " OK.\n"; // << e.getMsg() << endl;
	} catch (IMapException e) {
		cout << " FAILED.\n";
		isFailed = true;
	} catch (...) {
		cout << " FAILED. wrong exception\n"; // both must be the same
		isFailed = true;
	}
	//////////////////////////////////////////////////

	/** next test */
	i++; //
	cout << "\t subtest " << i << "\t";
	xmlMap.deinit();
	int inpDim = 2;
	int outDim = inpDim;
	int nmbLinks = 10;
	delete link;
	link = new float[inpDim + outDim];
	xmlMap.init(inpDim, inpDim, 1, 200);
	for (int k = 0; k < nmbLinks; k++) {
		xmlMap.addLink(link, link);
	};
	mem.deinitMap();
	mem.initMap(&xmlMap);
	///////////////////////////////////////////////////
	if (mem.getNmbLinks() == xmlMap.nmbOfLinks()) {
		cout << " OK.\n"; // << e.getMsg() << endl;
	} else {
		cout << " FAILED.\n" << mem.getNmbLinks() << " != "
				<< xmlMap.nmbOfLinks() << " != " << nmbLinks << endl;
		isFailed = true;
	};
	//////////////////////////////////////////////////

	/** next test */
	i++; //
	cout << "\t subtest " << i << "\t";
	///////////////////////////////////////////////////
	if ((mem.getDim() == inpDim) && (xmlMap.getInpDim() == xmlMap.getOutDim())
			&& (inpDim == xmlMap.getOutDim())) {
		cout << " OK.\n"; // << e.getMsg() << endl;
	} else {
		cout << " FAILED.\n" << "dims: " << mem.getDim() << " " << inpDim << " "
				<< xmlMap.getInpDim() << " " << xmlMap.getOutDim() << endl;
		isFailed = true;
	};
	//////////////////////////////////////////////////

	/** next test */
	i++; //
	cout << "\t subtest " << i << "\t";
	///////////////////////////////////////////////////
	if ((mem.getDim() == inpDim) && (xmlMap.getInpDim() == xmlMap.getOutDim())
			&& (inpDim == xmlMap.getOutDim())) {
		cout << " OK.\n"; // << e.getMsg() << endl;
	} else {
		cout << " FAILED.\n" << "dims: " << mem.getDim() << " " << inpDim << " "
				<< xmlMap.getInpDim() << " " << xmlMap.getOutDim() << endl;
		isFailed = true;
	};
	//////////////////////////////////////////////////

	/** next test */
	i++; //
	cout << "\t subtest " << i << "\t";
	// setup test
	try {
		mem.deinitMap();
		xmlMap.deinit();
		xmlMap.init(inpDim, inpDim, 1, 200);
	} catch (IMapException e) {
		cout << "terminate process: " << e.getMsg() << endl;
		exit(0);
	} catch (...) {
		cout << "terminate process: reason is an unknown error\n " << endl;
		exit(0);
	};
	float *inpN = new float[inpDim];
	float *outN = new float[outDim];
	nmbLinks = 10;
	for (int k = 0; k < nmbLinks; k++) {
		for (int l = 0; l < inpDim; l++) {
			inpN[l] = ((k * (inpDim + outDim)) + l);
		};
		for (int l = 0; l < inpDim; l++) {
			outN[l] = ((k * (inpDim + outDim)) + inpDim + l);
		};
		xmlMap.addLink(inpN, outN);
	};
	try {
		mem.initMap(&xmlMap);
	} catch (IMapException e) {
		cout << "terminate process: " << e.getMsg() << endl;
		exit(0);
	} catch (...) {
		cout << "terminate process: reason is an unknown error\n " << endl;
		exit(0);
	};
	///////////////////////////////////////////////////
	bool correctSum = true;
	for (int k = 0; k < nmbLinks; k++) {
		for (int l = 0; l < inpDim; l++) {
			inpN[l] = ((k * (inpDim + outDim)) + l);
		};

		try {
			mem.updateActivation(1, inpN);
		} catch (IMapException e) {
			cout << "terminate process while activation update :" << e.getMsg()
					<< endl;
			exit(0);
		} catch (...) {
			cout
					<< "terminate process while activation update : reason is an unknown error\n "
					<< endl;
			exit(0);
		};

		if (mem.sumActivation() != 1) {
			correctSum = false;
		}
	};

	if (correctSum) {
		cout << " OK.\n"; // << e.getMsg() << endl;
	} else {
		cout << " FAILED.\n" << mem.getNmbLinks() << " != "
				<< xmlMap.nmbOfLinks() << " != " << nmbLinks << endl;
		isFailed = true;
	};
	//////////////////////////////////////////////////

	/** next test */
	i++; //
	cout << "\t subtest " << i << "\t";
	///////////////////////////////////////////////////
	srand(time(NULL));
	int nmbInpNodes = ((int) (nmbLinks * ((float) rand()) / ((float) RAND_MAX)));
	int sum = 0;
	for (int k = 0; k < nmbInpNodes; k++) {
		for (int l = 0; l < inpDim; l++) {
			inpN[l] = ((k * (inpDim + outDim)) + l);
		};

		try {
			mem.updateActivation(1, inpN);
		} catch (IMapException e) {
			cout << "terminate process while activation update :" << e.getMsg()
					<< endl;
			exit(0);
		} catch (...) {
			cout
					<< "terminate process while activation update : reason is an unknown error\n "
					<< endl;
			exit(0);
		};

		// .sumActivation() is always 1 because all
		// links are only 1-to-1 mappings
		sum = sum + mem.sumActivation();
		//cout << sum << endl;
	};

	if (sum == nmbInpNodes) {
		cout << " OK.\n"; //  << "sum: " << sum << "  nmbNodes: " << nmbInpNodes << endl;
	} else {
		cout << " FAILED.\n";
		isFailed = true;
	};
	//////////////////////////////////////////////////

	/** next test */
	i++; //
	cout << "\t subtest " << i << "\t";
	// setup test
	try {
		mem.deinitMap();
		xmlMap.deinit();
		xmlMap.init(inpDim, inpDim, 1, 200);
	} catch (IMapException e) {
		cout << "terminate process: " << e.getMsg() << endl;
		exit(0);
	} catch (...) {
		cout << "terminate process: reason is an unknown error\n " << endl;
		exit(0);
	};
	inpN = new float[inpDim];
	outN = new float[outDim];
	nmbLinks = 100;
	nmbInpNodes = ((int) (nmbLinks * ((float) rand()) / ((float) RAND_MAX)));
	for (int k = 0; k < nmbInpNodes; k++) {
		for (int l = 0; l < inpDim; l++) {
			// input node will always be the same
			inpN[l] = 1;
		};
		for (int l = 0; l < inpDim; l++) {
			outN[l] = ((k * (inpDim + outDim)) + inpDim + l);
		};
		xmlMap.addLink(inpN, outN);
	};
	try {
		mem.initMap(&xmlMap);
	} catch (IMapException e) {
		cout << "terminate process: " << e.getMsg() << endl;
		exit(0);
	} catch (...) {
		cout << "terminate process: reason is an unknown error\n " << endl;
		exit(0);
	};
	///////////////////////////////////////////////////
	for (int l = 0; l < inpDim; l++) {
		inpN[l] = 1;
	};

	try {
		mem.updateActivation(1, inpN);
	} catch (IMapException e) {
		cout << "terminate process while activation update :" << e.getMsg()
				<< endl;
		exit(0);
	} catch (...) {
		cout
				<< "terminate process while activation update : reason is an unknown error\n "
				<< endl;
		exit(0);
	};

	sum = mem.sumActivation();

	if (sum == mem.getNmbLinks()) {
		cout << " OK.\n";
	} else {
		cout << " FAILED.\n" << mem.getNmbLinks() << " != "
				<< xmlMap.nmbOfLinks() << " != " << sum << endl;
		isFailed = true;
	};
	//////////////////////////////////////////////////

	/** next test */
	i++; //
	cout << "\t subtest " << i << "\t";
	// setup test

	try {
		mem.deinitMap();
		xmlMap.deinit();
		xmlMap.init(inpDim, inpDim, 1, 200);
	} catch (IMapException e) {
		cout << "terminate process: " << e.getMsg() << endl;
		exit(0);
	} catch (...) {
		cout << "terminate process: reason is an unknown error\n " << endl;
		exit(0);
	};

	inpN = new float[inpDim];
	outN = new float[outDim];
	nmbLinks = 10;
	// insert nmbLinks connections,
	// and all have the same output node
	int c = 0;
	for (int k = 0; k < nmbLinks; k++) {
		for (int l = 0; l < inpDim; l++) {
			inpN[l] = c;
			c++;
		};
		for (int l = 0; l < inpDim; l++) {
			// output node will always be the same
			outN[l] = 1;
		};
		xmlMap.addLink(inpN, outN);
	};

	try {
		mem.initMap(&xmlMap);
	} catch (IMapException e) {
		cout << "terminate process: " << e.getMsg() << endl;
		exit(0);
	} catch (...) {
		cout << "terminate process: reason is an unknown error\n " << endl;
		exit(0);
	};

	//xmlMap.writeXmlFile("lin.xml");
	///////////////////////////////////////////////////
	//generate a list of input nodes;
	nmbInpNodes = ((int) (nmbLinks * ((float) rand()) / ((float) RAND_MAX)));
	if (nmbInpNodes < 1)
		nmbInpNodes = 1;
	if (nmbInpNodes == 2)
		nmbInpNodes = nmbLinks;
	float *nodes = new float[nmbInpNodes * inpDim];
	c = 0;
	for (int k = 0; k < nmbInpNodes; k++) {
		for (int l = 0; l < inpDim; l++) {
			nodes[c] = c;
			c++;
		};
	}

	try {
		mem.updateActivation(nmbInpNodes, nodes);
	} catch (IMapException e) {
		cout << "terminate process while activation update :" << e.getMsg()
				<< endl;
		exit(0);
	} catch (...) {
		cout
				<< "terminate process while activation update : reason is an unknown error\n "
				<< endl;
		exit(0);
	};

	sum = mem.sumActivation();

	if (sum == nmbInpNodes) {
		cout << " OK. " << xmlMap.nmbOfLinks() << " = " << mem.getNmbLinks()
				<< " ---> " << nmbInpNodes << " == " << sum << endl << "\n";
	} else {
		cout << " FAILED.\n" << mem.getNmbLinks() << " != "
				<< xmlMap.nmbOfLinks() << " != " << sum << endl;
		isFailed = true;
	};
	//////////////////////////////////////////////////

	// ENDE TESTING ///
	if (isFailed) {
		// some test(s) failed.
		cout << "ERROR!!! Some tests of unit " << unitName << " failed. \n\n";
	} else {
		// closing all test successfully done.
		cout << "unit " << unitName << " successfully tested.\n\n";
	}

	exit(0);
}
;

