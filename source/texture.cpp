#include "texture.h"

#include "load_manager.h"

Texture::Texture() : Texture(NULL){}
Texture::Texture(Image * _img) : img(_img){}

void Texture::load(){
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLint mode = GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, mode, img->width,
        img->height, 0, mode, GL_UNSIGNED_BYTE,
        img->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, NULL);
}

GLuint Texture::getTexture(){
    return texture;
}



std::map<std::string, Texture *> TextureManager::textures;

void TextureManager::addTexture(std::string name, Texture * tex){
    if (textures.find(name) == textures.end()){
        textures[name] = tex;
    }
}

Texture * TextureManager::getTexture(std::string name){
    if (textures.find(name) == textures.end()){
        addTexture(name, new Texture(LoadManager::getImage(name)));
        textures[name]->load();
    }
    return textures[name];
}

