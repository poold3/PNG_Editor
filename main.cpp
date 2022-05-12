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

#include "C_IHDR.h"
#include "C_IDAT.h"
#include "C_bKGD.h"

using namespace std;

enum chunkType {
    IHDR, IEND, IDAT, PLTE, bKGD, cHRM, gAMA, iCCP, sBIT, sRGB,
    hIST, tRNS, pHYs, sPLT, tIME, iTXt, tEXt, zTXt, UNKNOWN,
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
    else if (sChunkType == "cHRM") {
        cout << "cHRM Chunk!" << endl;
        currentChunkType = cHRM;
    }
    else if (sChunkType == "gAMA") {
        cout << "gAMA Chunk!" << endl;
        currentChunkType = gAMA;
    }
    else if (sChunkType == "iCCP") {
        cout << "iCCP Chunk!" << endl;
        currentChunkType = iCCP;
    }
    else if (sChunkType == "sBIT") {
        cout << "sBIT Chunk!" << endl;
        currentChunkType = sBIT;
    }
    else if (sChunkType == "sRGB") {
        cout << "sRGB Chunk!" << endl;
        currentChunkType = sRGB;
    }
    else if (sChunkType == "hIST") {
        cout << "hIST Chunk!" << endl;
        currentChunkType = hIST;
    }
    else if (sChunkType == "tRNS") {
        cout << "tRNS Chunk!" << endl;
        currentChunkType = tRNS;
    }
    else if (sChunkType == "pHYs") {
        cout << "pHYs Chunk!" << endl;
        currentChunkType = pHYs;
    }
    else if (sChunkType == "sPLT") {
        cout << "sPLT Chunk!" << endl;
        currentChunkType = sPLT;
    }
    else if (sChunkType == "tIME") {
        cout << "tIME Chunk!" << endl;
        currentChunkType = tIME;
    }
    else if (sChunkType == "iTXt") {
        cout << "iTXt Chunk!" << endl;
        currentChunkType = iTXt;
    }
    else if (sChunkType == "tEXt") {
        cout << "tEXt Chunk!" << endl;
        currentChunkType = tEXt;
    }
    else if (sChunkType == "zTXt") {
        cout << "zTXt Chunk!" << endl;
        currentChunkType = zTXt;
    }
    else {
        stringstream error;
        error << "UNKNOWN Chunk: " << sChunkType << endl;
        throw std::invalid_argument(error.str());
    }
    return currentChunkType;
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

            if (DetermineChunkType(chunkAsBytes) != IHDR) {
                throw std::invalid_argument("The IHDR chunk is not the first chunk in the file!");
            }

            C_IHDR ihdr(chunkAsBytes);
            printf("%u\n", ihdr.GetCompressionType());
            vector<C_IDAT> idats;
            C_bKGD bkgd;

            //Size of IHDR chunk is always 25 bytes. Locate next chunk
            chunkAsBytes = chunkAsBytes + ihdr.GetChunkLength();
            
            chunkType currentChunkType = DetermineChunkType(chunkAsBytes);
            while (currentChunkType != IEND) {
                unsigned int chunkLength = DetermineChunkLength(chunkAsBytes);

                if (currentChunkType == IDAT) {
                    idats.push_back(C_IDAT(chunkAsBytes, chunkLength));
                }

                else if (currentChunkType == bKGD) {
                    if (bkgd.GetChunkLength() != 0) {
                        throw std::invalid_argument("Invalid PNG file! Too many bKGD chunks!");
                    }
                    bkgd = C_bKGD(chunkAsBytes, chunkLength);
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