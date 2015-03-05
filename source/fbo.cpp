#include "fbo.h"

#include <iostream>

FBO::FBO(){
}

FBO::FBO(int _width, int _height, int _layers, bool _depth): 
layersSize(_layers), depth(_depth), width(_width), height(_height){
}

void FBO::init(){
    textures = new GLuint[layersSize];

    glGenFramebuffers(1, &id); // Create Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, id); // Bind framebuffer for drawing

    if (id == 0){
        std::cout << "Could not create FBO" << std::endl;
    }

    for (int i = 0; i < layersSize; i++){
        addLayer(i);
    }

    if (depth){
        createDepth();
    }

    GLenum * attchs = new GLenum[layersSize];
    for (int i = 0; i < layersSize; i++){
        attchs[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    glDrawBuffers(layersSize, attchs);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum e = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (e != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Problem in FBO" << std::endl;

    printInfo();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int FBO::addLayer(int layerID){
    glGenTextures(1, &textures[layerID]);
    glBindTexture(GL_TEXTURE_2D, textures[layerID]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + layerID, textures[layerID], 0);

    return layerID;
}

void FBO::createDepth(){
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

GLuint FBO::getTexture(int texID){
    if (texID >= layersSize || texID < 0){
        std::cout << "Inexisting Layer " << texID << std::endl;
        return -1;
    }
    return textures[texID];
}

void FBO::enable(){
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void FBO::disable(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::printInfo() {
    //Code modified from:
    //      http://www.lighthouse3d.com/tutorials/glsl-core-tutorial/fragment-shader/

    glBindFramebuffer(GL_FRAMEBUFFER, id);

    int res;

    GLint buffer;
    int i = 0;
    do {
        glGetIntegerv(GL_DRAW_BUFFER0 + i, &buffer);

        if (buffer != GL_NONE) {

            printf("Shader Output Location %d - color attachment %d\n", i, buffer - GL_COLOR_ATTACHMENT0);

            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &res);
            printf("\tAttachment Type: %s\n", res == GL_TEXTURE ? "Texture" : "Render Buffer");
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &res);
            printf("\tAttachment object name: %d\n", res);
        }
        ++i;

    } while (buffer != GL_NONE);
}