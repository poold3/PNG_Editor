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

        unsigned int imageWidth;
        unsigned int imageHeight;
        unsigned char bitDepth;
        unsigned char colorType;
        unsigned char filterType;
        ParseIHDR(pngAsBytes + 8, imageWidth, imageHeight, bitDepth, colorType, filterType);
        

        

        delete[] pngAsBytes;
    }


    return 0;
}