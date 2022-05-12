#ifndef C_IDAT_H
#define C_IDAT_H

#include <iostream>
#include <string>
#include <sstream>
using namespace std;

class C_IDAT {
	private:
        char * chunkAsBytes;
        unsigned int chunkLength;
        unsigned int dataLength;

	public:
        C_IDAT (char * chunkAsBytes) {
            this->chunkAsBytes = chunkAsBytes;
            this->dataLength = htonl(*(unsigned int *)(chunkAsBytes));
            this->chunkLength = this->dataLength + 12;
        }
        C_IDAT (char * chunkAsBytes, unsigned int chunkLength) {
            this->chunkAsBytes = chunkAsBytes;
            this->chunkLength = chunkLength;
            this->dataLength = htonl(*(unsigned int *)(chunkAsBytes));
        }
        C_IDAT () {}

        char * GetChunkAsBytes () {
            return chunkAsBytes;
        }

        unsigned int GetChunkLength () {
            return chunkLength;
        }

        unsigned int GetDataLength () {
            return dataLength;
        }

        char * GetDataAsBytes () {
            return chunkAsBytes + 8;
        }

};
#endif