#include "render.h"

#include <iostream>

#include "GLIncludes.h"

#include "shader.h"
#include "load_manager.h"

namespace Render{
    Processor::Processor(){}

    Processor::Processor(int _width, int _height, int _nTextures, bool _depth, int _nFBOs, glm::vec4 _refreshColor) :
        width(_width), height(_height), nTextures(_nTextures), depth(_depth),
        nFBOs(_nFBOs), refreshColor(_refreshColor){}

    void Processor::init(){
        for (int i = 0; i < nFBOs; i++){
            fbos.push_back(new FBO(width, height, nTextures, depth));
            fbos[i]->init();
        }
    }

    void Processor::displayTexture(int fboID, int texID){
        if (fboID < 0 || fboID >= fbos.size()){
            std::cout << "Could not found fbo " << fboID << "\n";
            return;
        }

        if (texID < 0 || texID >= nTextures){
            std::cout << "Texture out of range: " << texID << "\n";
            return;
        }

        int planeVerticesSize = 6;
        int nComponents = 3;
        float planeVertices[] = {
            -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,

            -1.0f,  1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
        };

        GLuint vertexBuf;
        glGenBuffers(1, &vertexBuf);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices),
            planeVertices, GL_STATIC_DRAW);

        Shader * shader = LoadManager::getShader("default-texture-vertex.glsl",
                                                 "default-texture-fragment.glsl");
        shader->loadHandle("aPosition");
        shader->loadHandle("uTexID");

        glUseProgram(shader->getId());

        glEnableVertexAttribArray(shader->getHandle("aPosition"));
        glVertexAttribPointer(shader->getHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

        glActiveTexture(GL_TEXTURE0 + texID);
        glBindTexture(GL_TEXTURE_2D, fbos[fboID]->getTexture(texID));
        glUniform1i(shader->getHandle("uTexID"), texID);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, planeVerticesSize);

        glDisableVertexAttribArray(shader->getHandle("aPosition"));

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }




    GeometryProcessor::GeometryProcessor(){}

    GeometryProcessor::GeometryProcessor(int _width, int _height, int _nTextures, glm::vec4 _refreshColor) :
    Processor(_width, _height, _nTextures, true, 1, _refreshColor){}

    void GeometryProcessor::pass(std::vector<Object3D *> objects){
        glClearColor(refreshColor.r, refreshColor.g, refreshColor.b, refreshColor.a);

        fbos[0]->enable();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (unsigned int i = 0; i < objects.size(); i++){
            objects[i]->draw();
        }

        fbos[0]->disable();
    }
}