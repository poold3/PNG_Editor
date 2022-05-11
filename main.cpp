#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdio.h>
#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

using namespace std;

enum chunkType {
    IHDR, IEND, IDAT, PLTE, bKGD, UNKNOWN,
};

unsigned int DetermineChunkLength(char * chunkAsBytes) {
    unsigned int chunkLength = 12;
    chunkLength += htonl(*(unsigned int *)(chunkAsBytes));
    return chunkLength;
}

chunkType DetermineChunkType(char * chunkAsBytes) {
    chunkType currentChunkType;
    string sChunkType;
    sChunkType.push_back(*(unsigned char *)(chunkAsBytes + 4));
    sChunkType.push_back(*(unsigned char *)(chunkAsBytes + 5));
    sChunkType.push_back(*(unsigned char *)(chunkAsBytes + 6));
    sChunkType.push_back(*(unsigned char *)(chunkAsBytes + 7));
    if (sChunkType == "IHDR") {
        cout << "IHDR Chunk!" << endl;
        currentChunkType = IHDR;
    }
    else if (sChunkType == "IEND") {
        cout << "IEND Chunk!" << endl;
        currentChunkType = IEND;
    }
    else if (sChunkType == "IDAT") {
        cout << "IDAT Chunk!" << endl;
        currentChunkType = IDAT;
    }
    else if (sChunkType == "PLTE") {
        cout << "PLTE Chunk!" << endl;
        currentChunkType = PLTE;
    }
    else if (sChunkType == "bKGD") {
        cout << "bKGD Chunk!" << endl;
        currentChunkType = bKGD;
    }
    else {
        cout << "UNKNOWN Chunk: " << sChunkType << endl;
        currentChunkType = UNKNOWN;
    }
    return currentChunkType;
}


void ParseIHDR(char * iHDRAsBytes, unsigned int &imageWidth, unsigned int &imageHeight, unsigned char &bitDepth, unsigned char &colorType, unsigned char &filterType) {
    //unsigned int dataLength = htonl(*(unsigned int *)(iHDRAsBytes));
    if (DetermineChunkType(iHDRAsBytes) != IHDR) {
        throw std::invalid_argument("The IHDR chunk is not the first chunk in the file!");
    }
    imageWidth = htonl(*(unsigned int *)(iHDRAsBytes + 8));
    imageHeight = htonl(*(unsigned int *)(iHDRAsBytes + 12));
    bitDepth = *(unsigned char *)(iHDRAsBytes + 16);
    colorType = *(unsigned char *)(iHDRAsBytes + 17);
    filterType = *(unsigned char *)(iHDRAsBytes + 19);
    printf("\tImage Width: %u pixels | Image Height: %u pixels\n", imageWidth, imageHeight);
    printf("\tbitDepth: %u | colorType: %u | filterType: %u\n", bitDepth, colorType, filterType);
}

void VerifyPNG(char * pngAsBytes) {
    //First 8 bytes of a png file are always those in valid array
    unsigned int valid [8] = {137, 80, 78, 71, 13, 10, 26, 10};
    unsigned int actual [8];
    for (int i = 0; i < 8; ++i) {
        actual[i] = (unsigned int)*(unsigned char *)(pngAsBytes + i);
    }
    for (int i = 0; i < 8; ++i) {
        if (valid[i] != actual[i]) {
            throw std::invalid_argument("Invalid PNG file! First 8 bytes are not correct!");
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return 0;
    }
    ifstream is(argv[1], ifstream::binary);
    if (is) {
        is.seekg (0, is.end);
        int length = is.tellg();
        is.seekg (0, is.beg);
        char * pngAsBytes = new char [length];
        // read data as a block:
        is.read (pngAsBytes,length);

        if (!is) {
            cout << "error: only " << is.gcount() << " could be read" << endl;
            return 0;
        }
        is.close();
        try {
            //pngAsBytes contains the entire file...
            VerifyPNG(pngAsBytes);

            char * chunkAsBytes = pngAsBytes + 8;

            unsigned int imageWidth;
            unsigned int imageHeight;
            unsigned char bitDepth;
            unsigned char colorType;
            unsigned char filterType;
            ParseIHDR(chunkAsBytes, imageWidth, imageHeight, bitDepth, colorType, filterType);

            //Size of IHDR chunk is always 25 bytes. Locate next chunk
            chunkAsBytes = chunkAsBytes + 25;
            
            chunkType currentChunkType = DetermineChunkType(chunkAsBytes);
            while (currentChunkType != IEND) {
                unsigned int chunkLength = DetermineChunkLength(chunkAsBytes);
                cout << "Length: " << chunkLength << " bytes!" << endl;
                if (currentChunkType == bKGD && (colorType == 2 || colorType == 6)) {
                    unsigned short int red = htons(*(unsigned short int *)(chunkAsBytes + 8));
                    unsigned short int green = htons(*(unsigned short int *)(chunkAsBytes + 10));
                    unsigned short int blue = htons(*(unsigned short int *)(chunkAsBytes + 12));
                    cout << "Background color to be used: rgb("<< red << "," << green << "," << blue << ")" << endl;
                }

                chunkAsBytes = chunkAsBytes + chunkLength;
                currentChunkType = DetermineChunkType(chunkAsBytes);
            }

        }
        catch(exception& e) {
            cout << endl << e.what() << endl;
            return 0;
        }

        delete[] pngAsBytes;
    }


    return 0;
}