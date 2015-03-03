#ifndef BMP_LOADER_H
#define BMP_LOADER_H

#include <string>
#include "GLSL.h"

/*Code extracted and modified from: 
http://www.cplusplus.com/articles/GwvU7k9E/

NOTE: Files must be 24-bit Bitmap format (BMP)*/

typedef unsigned char Uint8;

class BMPImage{
public:
    BMPImage();
    BMPImage(std::string _path);

    int load();

    std::string path;
    Uint8* pixels;
    GLuint width, height;
};

#endif