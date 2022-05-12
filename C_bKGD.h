#ifndef C_BKGD_H
#define C_BKGD_H

#include <iostream>
#include <string>
#include <sstream>
using namespace std;

class C_bKGD {
	private:
        char * chunkAsBytes;
        unsigned int chunkLength;
        unsigned int dataLength;

	public:
        C_bKGD (char * chunkAsBytes) {
            this->chunkAsBytes = chunkAsBytes;
            this->dataLength = htonl(*(unsigned int *)(chunkAsBytes));
            this->chunkLength = this->dataLength + 12;
        }
        C_bKGD (char * chunkAsBytes, unsigned int chunkLength) {
            this->chunkAsBytes = chunkAsBytes;
            this->chunkLength = chunkLength;
            this->dataLength = htonl(*(unsigned int *)(chunkAsBytes));
        }
        C_bKGD () {}

        char * GetChunkAsBytes () {
            return chunkAsBytes;
        }

        unsigned int GetChunkLength () {
            return chunkLength;
        }

        unsigned int GetDataLength () {
            return dataLength;
        }

        // if (currentChunkType == bKGD && (colorType == 2 || colorType == 6)) {
        //     unsigned short int red = htons(*(unsigned short int *)(chunkAsBytes + 8));
        //     unsigned short int green = htons(*(unsigned short int *)(chunkAsBytes + 10));
        //     unsigned short int blue = htons(*(unsigned short int *)(chunkAsBytes + 12));
        //     cout << "Background color to be used: rgb("<< red << "," << green << "," << blue << ")" << endl;
        // }

};
#endif