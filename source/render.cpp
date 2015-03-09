#include "render.h"

#include <iostream>

#include "glm/gtc/type_ptr.hpp"

#include "load_manager.h"
#include "virtual_camera.h"

namespace Render{
    Processor::Processor(){}

    Processor::Processor(int _width, int _height, int _nTextures, bool _depth, int _nFBOs, glm::vec4 _refreshColor) :
        width(_width), height(_height), nTextures(_nTextures), depth(_depth),
        nFBOs(_nFBOs), refreshColor(_refreshColor){
        outFBO = nullptr;
    }

    void Processor::init(){
        for (int i = 0; i < nFBOs; i++){
            fbos.push_back(new FBO(width, height, nTextures, depth));
            fbos[i]->init();
        }

        if (nFBOs >= 1){
            outFBO = fbos[0];
        }

        prepareDisplay();
    }

    void Processor::prepareDisplay(){
        int planeVerticesSize = 6;
        int nComponents = 3;
        float planeVertices[] = {
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
        };

        glGenBuffers(1, &vertexBuf);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices),
            planeVertices, GL_STATIC_DRAW);

        shader = LoadManager::getShader("default-texture-vertex.glsl",
            "default-texture-fragment.glsl");
        shader->loadHandle("aPosition");
        shader->loadHandle("uTexID");

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Processor::displayTextures(std::vector<int> texIDs){
        if (!hasOutput()){
            return;
        }

        glViewport(0, 0, width, height);

        glUseProgram(shader->getId());

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glEnableVertexAttribArray(shader->getHandle("aPosition"));
        glVertexAttribPointer(shader->getHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

        for (unsigned int i = 0; i < texIDs.size(); i++){
            if (texIDs[i] < 0 || texIDs[i] >= nTextures){
                std::cout << "Texture out of range: " << texIDs[i] << "\n";
                return;
            }

            glActiveTexture(GL_TEXTURE0 + texIDs[i]);
            glBindTexture(GL_TEXTURE_2D, outFBO->getTexture(texIDs[i]));
            glUniform1i(shader->getHandle("uTexID"), texIDs[i]);
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(shader->getHandle("aPosition"));
        
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }

    void Processor::displayTexture(int texID){
        std::vector<int> v(1, texID);
        displayTextures(v);
    }

    bool Processor::hasOutput(){
        return outFBO != nullptr;
    }

    FBO * Processor::getOutFBO(){
        return outFBO;
    }




    GeometryProcessor::GeometryProcessor(){}

    GeometryProcessor::GeometryProcessor(int _width, int _height, int _nTextures, glm::vec4 _refreshColor) :
    Processor(_width, _height, _nTextures, true, 1, _refreshColor){}

    void GeometryProcessor::pass(std::vector<Object3D *> objects){
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glViewport(0, 0, width, height);
        glClearColor(refreshColor.r, refreshColor.g, refreshColor.b, refreshColor.a);

        fbos[0]->enable();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (unsigned int i = 0; i < objects.size(); i++){
            objects[i]->draw();
        }

        fbos[0]->disable();
    }




    LightProcessor::LightProcessor(){}
    LightProcessor::LightProcessor(int _width, int _height, int _nTextures) :
        Processor(_width, _height, _nTextures, false, 1, glm::vec4(0,0,0,1)){}

    void LightProcessor::init(){
        Processor::init();

        lightShader = LoadManager::getShader("default-texture-vertex.glsl", 
                                             "default-lighting-fragment.glsl");
        lightShader->loadHandle("aPosition");
        lightShader->loadHandle("uDiffuseID");
        lightShader->loadHandle("uPositionID");
        lightShader->loadHandle("uNormalID");
        lightShader->loadHandle("uSpecularID");
        lightShader->loadHandle("uAmbientID");
        lightShader->loadHandle("uViewMatrix");
        lightShader->loadHandle("uEye");
        lightShader->loadHandle("uLightPos");
        lightShader->loadHandle("uLightFallOff");
        lightShader->loadHandle("uLightColor");
    }

    void LightProcessor::pass(Processor * processor, std::list<Light *> lights){
        glViewport(0, 0, width, height);
        glClearColor(refreshColor.r, refreshColor.g, refreshColor.b, refreshColor.a);

        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);

        fbos[0]->enable();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(lightShader->getId());

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glEnableVertexAttribArray(lightShader->getHandle("aPosition"));
        glVertexAttribPointer(lightShader->getHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);


        glUniform3f(lightShader->getHandle("uEye"),
            CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);

        glUniformMatrix4fv(lightShader->getHandle("uViewMatrix"), 1, GL_FALSE,
            glm::value_ptr(CamManager::currentCam()->getViewMatrix()));

        for (std::list<Light *>::iterator it = lights.begin(); it != lights.end(); it++){
            (*it)->load(lightShader->getHandle("uLightPos"),
                        lightShader->getHandle("uLightColor"),
                        lightShader->getHandle("uLightFallOff"));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(0));
            glUniform1i(lightShader->getHandle("uDiffuseID"), 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(2));
            glUniform1i(lightShader->getHandle("uPositionID"), 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(3));
            glUniform1i(lightShader->getHandle("uNormalID"), 2);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(4));
            glUniform1i(lightShader->getHandle("uSpecularID"), 3);

            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(5));
            glUniform1i(lightShader->getHandle("uAmbientID"), 4);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        fbos[0]->disable();
        
        glDisableVertexAttribArray(lightShader->getHandle("aPosition"));

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }





    PostProcessor::PostProcessor(){}
    PostProcessor::PostProcessor(int _width, int _height, int _nTextures, int _nFBOs):
        Processor(_width, _height, _nTextures, false, _nFBOs, glm::vec4(0, 0, 0, 1)){}

    void PostProcessor::passBlur(Processor * processor, int cycles, Shader * blurShader){
        if (nFBOs < 2){
            std::cout << "There isn't FBOs enough\n";
            return;
        }

        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);

        glViewport(0, 0, width, height);
        glUseProgram(blurShader->getId());
        glClearColor(refreshColor.r, refreshColor.g, refreshColor.b, refreshColor.a);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glEnableVertexAttribArray(blurShader->getHandle("aPosition"));
        glVertexAttribPointer(blurShader->getHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

        outFBO = processor->getOutFBO();
        int alphaMapTexID = 1;
        for (int cycle = 0; cycle < cycles; cycle++){
            for (int i = 0; i < 2; i++){
                fbos[i]->enable();

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, outFBO->getTexture(alphaMapTexID));
                glUniform1i(blurShader->getHandle("uTexID"), 0);

                if (i == 0)
                    glUniform2f(blurShader->getHandle("uDir"), 1, 0);
                else
                    glUniform2f(blurShader->getHandle("uDir"), 0, 1);

                glDrawArrays(GL_TRIANGLES, 0, 6);

                fbos[i]->disable();

                outFBO = fbos[i];
                alphaMapTexID = 0;
            }
        }

        glDisableVertexAttribArray(blurShader->getHandle("aPosition"));

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }


    void PostProcessor::passBloom(Processor * processorAlpha, Processor * processorDiffuse, 
        Shader * bloomShader, Shader * blurShader, int cycles){
        passBlur(processorAlpha, cycles, blurShader);

        glViewport(0, 0, width, height);

        glUseProgram(bloomShader->getId());
        glClearColor(refreshColor.r, refreshColor.g, refreshColor.b, refreshColor.a);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glEnableVertexAttribArray(bloomShader->getHandle("aPosition"));
        glVertexAttribPointer(bloomShader->getHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

        fbos[0]->enable();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, processorDiffuse->getOutFBO()->getTexture(0));
        glUniform1i(bloomShader->getHandle("uTexID"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, outFBO->getTexture(0));
        glUniform1i(bloomShader->getHandle("uBlurTexID"), 1);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        fbos[0]->disable();
        outFBO = fbos[0];

        glDisableVertexAttribArray(bloomShader->getHandle("aPosition"));

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }
}