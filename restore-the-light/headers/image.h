#ifndef BMP_LOADER_H
#define BMP_LOADER_H

#include <string>
#include "GLIncludes.h"

/*Code extracted and modified from: Zoe Wood

NOTE: Files must be 24-bit Bitmap format (BMP)*/

typedef unsigned char Uint8;

class Image{
public:
    Image();
    Image(std::string _path);

    int loadBMP();

    std::string path;
    char * pixels;
    GLuint width, height;
};

#endif