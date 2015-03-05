#ifndef BMP_LOADER_H
#define BMP_LOADER_H

#include <string>
#include "GLIncludes.h"

/*Code extracted and modified from: 
http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/

NOTE: Files must be 24-bit Bitmap format (BMP)*/

typedef unsigned char Uint8;

class Image{
public:
    Image();
    Image(std::string _path);

    int loadBMP();

    std::string path;
    Uint8* pixels;
    GLuint width, height;
};

#endif