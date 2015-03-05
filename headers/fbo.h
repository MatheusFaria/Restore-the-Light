#ifndef __FBO_H__
#define __FBO_H__

#include "GLIncludes.h"

class FBO{
public:
    FBO();
    FBO(int _width, int _height, int _layers, bool _depth);

    void init();

    GLuint getTexture(int texID);

    void enable();
    void disable();

    void printInfo();

private:
    int layersSize, 
        width, height;
    bool depth;

    GLuint id, * textures, depthBuf;


    int addLayer(int layerID);
    void createDepth();
};

#endif