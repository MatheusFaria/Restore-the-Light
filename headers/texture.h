#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <string>
#include <map>

#include "image.h"

class Texture{
public:
    Texture();
    Texture(Image * _img);

    void load();
    GLuint getTexture();

private:
    Image * img;
    GLuint texture;
};

class TextureManager{
public:
    static void addTexture(std::string name, Texture * tex);
    static Texture * getTexture(std::string name);

private:
    static std::map<std::string, Texture *> textures;
};

#endif