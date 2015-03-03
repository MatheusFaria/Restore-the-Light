#ifndef BMP_LOADER_H
#define BMP_LOADER_H

#include <string>
#include "GLSL.h"

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