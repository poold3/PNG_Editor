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
        unsigned int width = htonl(*(unsigned int *)(pngAsBytes + 16));
        unsigned int height = htonl(*(unsigned int *)(pngAsBytes + 20));
        unsigned char bitDepth = *(unsigned char *)(pngAsBytes + 24);
        unsigned char colorType = *(unsigned char *)(pngAsBytes + 25);
        unsigned char filterType = *(unsigned char *)(pngAsBytes + 27);
        printf("%d %d %d %d %d\n", width, height, bitDepth, colorType, filterType);
        

        delete[] pngAsBytes;
    }


    return 0;
}