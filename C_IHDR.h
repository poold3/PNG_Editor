#ifndef C_IHDR_H
#define C_IHDR_H

#include <iostream>
#include <string>
#include <sstream>
using namespace std;

class C_IHDR {
	private:
        char * chunkAsBytes;
        unsigned int chunkLength;
        unsigned int dataLength;
        unsigned int imageWidth;
        unsigned int imageHeight;
        unsigned char bitDepth;
        unsigned char colorType;
        unsigned char compressionType;
        unsigned char filterType;

	public:
        C_IHDR (char * chunkAsBytes) {
            this->chunkAsBytes = chunkAsBytes;
            this->dataLength = htonl(*(unsigned int *)(chunkAsBytes));
            this->chunkLength = this->dataLength + 12;
            this->imageWidth = htonl(*(unsigned int *)(chunkAsBytes + 8));
            this->imageHeight = htonl(*(unsigned int *)(chunkAsBytes + 12));
            this->bitDepth = *(unsigned char *)(chunkAsBytes + 16);
            this->colorType = *(unsigned char *)(chunkAsBytes + 17);
            this->compressionType = *(unsigned char *)(chunkAsBytes + 18);
            this->filterType = *(unsigned char *)(chunkAsBytes + 19);
        }
        C_IHDR (char * chunkAsBytes, unsigned int chunkLength) {
            this->chunkAsBytes = chunkAsBytes;
            this->dataLength = htonl(*(unsigned int *)(chunkAsBytes));
            this->chunkLength = chunkLength;
            this->imageWidth = htonl(*(unsigned int *)(chunkAsBytes + 8));
            this->imageHeight = htonl(*(unsigned int *)(chunkAsBytes + 12));
            this->bitDepth = *(unsigned char *)(chunkAsBytes + 16);
            this->colorType = *(unsigned char *)(chunkAsBytes + 17);
            this->compressionType = *(unsigned char *)(chunkAsBytes + 18);
            this->filterType = *(unsigned char *)(chunkAsBytes + 19);
        }
        C_IHDR () {}

        char * GetChunkAsBytes () {
            return chunkAsBytes;
        }

        unsigned int GetChunkLength () {
            return chunkLength;
        }

        unsigned int GetDataLength () {
            return dataLength;
        }

        unsigned int GetImageWidth () {
            return imageWidth;
        }

        unsigned int GetImageHeight () {
            return imageHeight;
        }

        unsigned char GetBitDepth () {
            return bitDepth;
        }

        unsigned char GetColorType () {
            return colorType;
        }

        unsigned char GetCompressionType () {
            return compressionType;
        }

        unsigned char GetFilterType () {
            return filterType;
        }

};
#endif