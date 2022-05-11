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

chunkType DetermineChunkType(char * chunkAsBytes, chunkType &currentChunkType) {
    unsigned char typeLetterOne = *(unsigned char *)(chunkAsBytes + 4);
    unsigned char typeLetterTwo = *(unsigned char *)(chunkAsBytes + 5);
    unsigned char typeLetterThree = *(unsigned char *)(chunkAsBytes + 6);
    unsigned char typeLetterFour = *(unsigned char *)(chunkAsBytes + 7);
    if (typeLetterOne == 'I' && typeLetterTwo == 'H' && typeLetterThree == 'D' && typeLetterFour == 'R') {
        cout << "IHDR Chunk!" << endl;
        currentChunkType = IHDR;
    }
    else if (typeLetterOne == 'I' && typeLetterTwo == 'E' && typeLetterThree == 'N' && typeLetterFour == 'D') {
        cout << "IEND Chunk!" << endl;
        currentChunkType = IEND;
    }
    else if (typeLetterOne == 'I' && typeLetterTwo == 'D' && typeLetterThree == 'A' && typeLetterFour == 'T') {
        cout << "IDAT Chunk!" << endl;
        currentChunkType = IDAT;
    }
    else if (typeLetterOne == 'P' && typeLetterTwo == 'L' && typeLetterThree == 'T' && typeLetterFour == 'E') {
        cout << "PLTE Chunk!" << endl;
        currentChunkType = PLTE;
    }
    else if (typeLetterOne == 'b' && typeLetterTwo == 'K' && typeLetterThree == 'G' && typeLetterFour == 'D') {
        cout << "bKGD Chunk!" << endl;
        currentChunkType = bKGD;
    }
    else {
        cout << "UNKNOWN Chunk: " << typeLetterOne << typeLetterTwo << typeLetterThree << typeLetterFour << endl;
        currentChunkType = UNKNOWN;
    }
    return currentChunkType;
}


void ParseIHDR(char * iHDRAsBytes, unsigned int &imageWidth, unsigned int &imageHeight, unsigned char &bitDepth, unsigned char &colorType, unsigned char &filterType) {
    unsigned int dataLength = htonl(*(unsigned int *)(iHDRAsBytes));
    unsigned char typeLetterOne = *(unsigned char *)(iHDRAsBytes + 4);
    unsigned char typeLetterTwo = *(unsigned char *)(iHDRAsBytes + 5);
    unsigned char typeLetterThree = *(unsigned char *)(iHDRAsBytes + 6);
    unsigned char typeLetterFour = *(unsigned char *)(iHDRAsBytes + 7);
    if (typeLetterOne != 'I' || typeLetterTwo != 'H' || typeLetterThree != 'D' || typeLetterFour != 'R') {
        //throw
        cout << "Invalid IHDR Chunk!" << endl;
    }
    imageWidth = htonl(*(unsigned int *)(iHDRAsBytes + 8));
    imageHeight = htonl(*(unsigned int *)(iHDRAsBytes + 12));
    bitDepth = *(unsigned char *)(iHDRAsBytes + 16);
    colorType = *(unsigned char *)(iHDRAsBytes + 17);
    filterType = *(unsigned char *)(iHDRAsBytes + 19);
    printf("%u, %c, %c, %c, %c, %u, %u, %u, %u, %u\n", dataLength, typeLetterOne, typeLetterTwo, typeLetterThree, typeLetterFour, imageWidth, imageHeight, bitDepth, colorType, filterType);
    return;
}

bool VerifyPNG(char * pngAsBytes) {
    unsigned int valid [8] = {137, 80, 78, 71, 13, 10, 26, 10};
    unsigned int actual [8];
    for (int i = 0; i < 8; ++i) {
        actual[i] = (unsigned int)*(unsigned char *)(pngAsBytes + i);
    }
    for (int i = 0; i < 8; ++i) {
        if (valid[i] != actual[i]) {
            return false;
        }
    }

    return true;
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

        // ...pngAsBytes contains the entire file...
        if (!VerifyPNG(pngAsBytes)) {
            cout << "Invalid PNG File!" << endl;
            return 0;
        }

        char * chunkAsBytes = pngAsBytes + 8;

        unsigned int imageWidth;
        unsigned int imageHeight;
        unsigned char bitDepth;
        unsigned char colorType;
        unsigned char filterType;
        ParseIHDR(chunkAsBytes, imageWidth, imageHeight, bitDepth, colorType, filterType);

        //Size of IHDR chunk is always 25 bytes
        chunkAsBytes = chunkAsBytes + 25;
        chunkType currentChunkType;
        while (DetermineChunkType(chunkAsBytes, currentChunkType) != IEND) {
            unsigned int chunkLength = DetermineChunkLength(chunkAsBytes);
            cout << "Length: " << chunkLength << " bytes!" << endl;
            if (currentChunkType == bKGD && (colorType == 2 || colorType == 6)) {
                unsigned short int red = htons(*(unsigned short int *)(chunkAsBytes + 8));
                unsigned short int green = htons(*(unsigned short int *)(chunkAsBytes + 10));
                unsigned short int blue = htons(*(unsigned short int *)(chunkAsBytes + 12));
                cout << "Background color to be used: rgb("<< red << "," << green << "," << blue << ")" << endl;
            }

            chunkAsBytes = chunkAsBytes + chunkLength;
        }
        

        delete[] pngAsBytes;
    }


    return 0;
}