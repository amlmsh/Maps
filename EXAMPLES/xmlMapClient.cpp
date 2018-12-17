/*
 This program is an implementation of a client connecting
 to a standard image data server getting and displaying
 the image data provided by the server.

 Copyright (C) 2009  Martin Huelse,
 http://aml.somebodyelse.de

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Socket.H"  // For Socket, ServerSocket, and SocketException
#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()
#include <sstream>

#include "CXmlMap.h"

TCPSocket *dataSource_ = NULL;
unsigned short SOURCE_SERVER_PORT_;
char *SOURCE_SERVER_ADR_;

string *createSendLine(CXmlMap *m);

/**
 *
 * @param argc number of command line parameter
 * @param *argv[] list of parameters
 */
int main(int argc, char *argv[]) {
	CXmlMap *map = NULL;
	srand(time(NULL));

	try {
		SOURCE_SERVER_ADR_ = argv[1];
		SOURCE_SERVER_PORT_ = atoi(argv[2]);
		map = new CXmlMap();
		map->readXmlFile(argv[3]);
	} catch (...) {
		cerr << "Can't read command line parameters, terminate process.\n";
		exit(0);
	};

	cout << "mapping dimension read: " << map->getInpDim() <<  "(inp. dim.) ";
	cout << map->getOutDim() << "(out.dim.)\n";

	// client for image data
	try {
		dataSource_ = new TCPSocket(SOURCE_SERVER_ADR_, SOURCE_SERVER_PORT_);
	} catch (...) {
		cout << "No server found.\n" << SOURCE_SERVER_ADR_ << endl << endl;
		exit(1);
	};

	int sizeEchoBufferMetaData = 100;
	char echoBufferMetaData[sizeEchoBufferMetaData];
	int bytesReceived;
	const char *sendLine;

	while (1) {
		// get meta data
		try {
			sendLine = (createSendLine(map))->c_str();
			cout << "line to be sent to server: '" << sendLine << "'\n";
			dataSource_->send(sendLine, strlen(sendLine));
		} catch (...) {
			cerr << "Error while sending: " << argv[3] << endl;
			exit(0);
		};

		if ((bytesReceived = dataSource_->recv(echoBufferMetaData,
				sizeEchoBufferMetaData)) <= 0) {
			cerr << "Can't get Standard Image Data Server Version, terminate process.\n";
			exit(0);
		};
		echoBufferMetaData[bytesReceived] = '\0';
		cout << "response: '" << echoBufferMetaData << "'\n\n\n";
	}

	delete dataSource_;
	exit(0);
}
;

string *createSendLine(CXmlMap *m) {
	int idx;
	float *link;
	int inpDim = m->getInpDim();


	idx = rand() % m->nmbOfLinks();
	link = m->ptrLinkByIndex(idx);
	stringstream ss;
	for (idx = 0; idx < inpDim; idx++) {
		ss << link[idx] << "\t";
	}
	ss << "\n";
	string result = ss.str();
	cout << "result: "  << result << endl;
	return new string(result);
}
