#include "render.h"

#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_PURE
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "load_manager.h"
#include "virtual_camera.h"

namespace Render{
    Processor::Processor(){}

    Processor::Processor(int _width, int _height, int _nTextures, bool _depth, int _nFBOs, glm::vec4 _refreshColor) :
        width(_width), height(_height), nTextures(_nTextures), depth(_depth),
        nFBOs(_nFBOs), refreshColor(_refreshColor){
        outFBO = NULL;
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
        float planeVertices[] = {
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
        };

        unsigned int idx[] = { 0, 1, 2, 0, 2, 3 };

        glGenBuffers(1, &vertexBuf);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

        glGenBuffers(1, &elementBuf);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuf);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

        shader = LoadManager::getShader(
            "texture-vertex.glsl",
            "texture-fragment.glsl");

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void Processor::displayTextures(std::vector<int> texIDs){
        if (!hasOutput()){
            return;
        }

        glViewport(0, 0, width, height);

        glUseProgram(shader->getId());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuf);

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

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(shader->getHandle("aPosition"));
        
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }

    void Processor::displayTexture(int texID){
        std::vector<int> v(1, texID);
        displayTextures(v);
    }

    bool Processor::hasOutput(){
        return outFBO != NULL;
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
        glClearColor(refreshColor.x, refreshColor.y, refreshColor.z, refreshColor.w);

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

        initShader();
        //initGeometry();
    }

    void LightProcessor::pass(Processor * processor, std::list<Light *> pointLights, 
                                std::list<Light *> spotLights, std::list<Light *> directionalLights){
        glViewport(0, 0, width, height);
        glClearColor(refreshColor.x, refreshColor.y, refreshColor.z, refreshColor.w);

        glEnable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);

        fbos[0]->enable();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        passPointLights(processor, pointLights);
        passSpotLights(processor, spotLights);
        passDirectionalLights(processor, directionalLights);
        passAmbientLight(processor);

        fbos[0]->disable();
        

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }


    void LightProcessor::passPointLights(Processor * processor, std::list<Light *> lights){
        /*glUseProgram(pointShader->getId());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glEnableVertexAttribArray(pointShader->getHandle("aPosition"));
        glVertexAttribPointer(pointShader->getHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

        glUniform3f(pointShader->getHandle("uEye"),
            CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);

        glUniform2f(pointShader->getHandle("uScreenSize"), 600, 600);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(0));
        glUniform1i(pointShader->getHandle("uDiffuseID"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(2));
        glUniform1i(pointShader->getHandle("uPositionID"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(3));
        glUniform1i(pointShader->getHandle("uNormalID"), 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(4));
        glUniform1i(pointShader->getHandle("uSpecularID"), 3);

        glm::mat4 MVP, 
                  VP = CamManager::currentCam()->projectionMatrix *
                       CamManager::currentCam()->getViewMatrix();
        Light * light;
        for (std::list<Light *>::iterator it = lights.begin(); it != lights.end(); it++){
            light = *it;

            glm::vec4 lightPos = CamManager::currentCam()->getViewMatrix() * glm::vec4(light->pos, 1);

            glUniform3f(pointShader->getHandle("uLightColor"), light->color.x, light->color.y, light->color.z);
            glUniform3f(pointShader->getHandle("uLightPos"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(pointShader->getHandle("uLightFallOff"), light->fallOff.x, light->fallOff.y, light->fallOff.z);

            MVP = VP *
                  glm::translate(glm::mat4(1.0f), light->pos) *
                  glm::scale(glm::mat4(1.0f), glm::vec3(10));
            glUniformMatrix4fv(pointShader->getHandle("uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));

            glDrawElements(GL_TRIANGLES, (int)pointLightArea->getIndices().size(), GL_UNSIGNED_INT, 0);
        }*/
        glUseProgram(pointShader->getId());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuf);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glEnableVertexAttribArray(pointShader->getHandle("aPosition"));
        glVertexAttribPointer(pointShader->getHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

        glUniform3f(pointShader->getHandle("uEye"),
            CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);

        glUniform2f(pointShader->getHandle("uScreenSize"), width, height);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(0));
        glUniform1i(pointShader->getHandle("uDiffuseID"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(2));
        glUniform1i(pointShader->getHandle("uPositionID"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(3));
        glUniform1i(pointShader->getHandle("uNormalID"), 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(4));
        glUniform1i(pointShader->getHandle("uSpecularID"), 3);

        Light * light;
        for (std::list<Light *>::iterator it = lights.begin(); it != lights.end(); it++){
            light = *it;

            glm::vec4 lightPos = CamManager::currentCam()->getViewMatrix() * glm::vec4(light->pos, 1);

            glUniform3f(pointShader->getHandle("uLightColor"), light->color.x, light->color.y, light->color.z);
            glUniform3f(pointShader->getHandle("uLightPos"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(pointShader->getHandle("uLightFallOff"), light->fallOff.x, light->fallOff.y, light->fallOff.z);

            glUniformMatrix4fv(pointShader->getHandle("uMVP"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
    }

    void LightProcessor::passSpotLights(Processor * processor, std::list<Light *> lights){
        /*glUseProgram(spotShader->getId());
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glEnableVertexAttribArray(spotShader->getHandle("aPosition"));
        glVertexAttribPointer(spotShader->getHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

        glUniform3f(spotShader->getHandle("uEye"),
            CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);

        glUniform2f(spotShader->getHandle("uScreenSize"), 600, 600);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(0));
        glUniform1i(spotShader->getHandle("uDiffuseID"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(2));
        glUniform1i(spotShader->getHandle("uPositionID"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(3));
        glUniform1i(spotShader->getHandle("uNormalID"), 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(4));
        glUniform1i(spotShader->getHandle("uSpecularID"), 3);

        glm::mat4 MVP,
            VP = CamManager::currentCam()->projectionMatrix *
            CamManager::currentCam()->getViewMatrix();
        Light * light;
        for (std::list<Light *>::iterator it = lights.begin(); it != lights.end(); it++){
            light = *it;

            glm::vec4 lightPos = CamManager::currentCam()->getViewMatrix() * glm::vec4(light->pos, 1);
            glm::vec4 lightDir = CamManager::currentCam()->getViewMatrix() * glm::vec4(light->dir, 0);

            glUniform3f(spotShader->getHandle("uLightColor"), light->color.x, light->color.y, light->color.z);
            glUniform3f(spotShader->getHandle("uLightPos"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(spotShader->getHandle("uLightFallOff"), light->fallOff.x, light->fallOff.y, light->fallOff.z);
            glUniform3f(spotShader->getHandle("uLightDir"), lightDir.x, lightDir.y, lightDir.z);
            glUniform1f(spotShader->getHandle("uLightCutOffAngle"), glm::cos(light->cutOffAngle));

            MVP = VP *
                glm::translate(glm::mat4(1.0f), light->pos) *
                glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
            glUniformMatrix4fv(spotShader->getHandle("uMVP"), 1, GL_FALSE, glm::value_ptr(MVP));

            glDrawElements(GL_TRIANGLES, (int)pointLightArea->getIndices().size(), GL_UNSIGNED_INT, 0);
        }*/

        glUseProgram(spotShader->getId());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuf);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glEnableVertexAttribArray(spotShader->getHandle("aPosition"));
        glVertexAttribPointer(spotShader->getHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

        glUniform3f(spotShader->getHandle("uEye"),
            CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);

        glUniform2f(spotShader->getHandle("uScreenSize"), width, height);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(0));
        glUniform1i(spotShader->getHandle("uDiffuseID"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(2));
        glUniform1i(spotShader->getHandle("uPositionID"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(3));
        glUniform1i(spotShader->getHandle("uNormalID"), 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(4));
        glUniform1i(spotShader->getHandle("uSpecularID"), 3);

        Light * light;
        for (std::list<Light *>::iterator it = lights.begin(); it != lights.end(); it++){
            light = *it;

            glm::vec4 lightPos = CamManager::currentCam()->getViewMatrix() * glm::vec4(light->pos, 1);
            glm::vec4 lightDir = CamManager::currentCam()->getViewMatrix() * glm::vec4(light->dir, 0);

            glUniform3f(spotShader->getHandle("uLightColor"), light->color.x, light->color.y, light->color.z);
            glUniform3f(spotShader->getHandle("uLightPos"), lightPos.x, lightPos.y, lightPos.z);
            glUniform3f(spotShader->getHandle("uLightFallOff"), light->fallOff.x, light->fallOff.y, light->fallOff.z);
            glUniform3f(spotShader->getHandle("uLightDir"), lightDir.x, lightDir.y, lightDir.z);
            glUniform1f(spotShader->getHandle("uLightCutOffAngle"), glm::cos(light->cutOffAngle));

            glUniformMatrix4fv(spotShader->getHandle("uMVP"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
    }

    void LightProcessor::passDirectionalLights(Processor * processor, std::list<Light *> lights){
        /*glUseProgram(directionalShader->getId());

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glEnableVertexAttribArray(directionalShader->getHandle("aPosition"));
        glVertexAttribPointer(directionalShader->getHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

        glUniform3f(directionalShader->getHandle("uEye"),
            CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(0));
        glUniform1i(directionalShader->getHandle("uDiffuseID"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(2));
        glUniform1i(directionalShader->getHandle("uPositionID"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(3));
        glUniform1i(directionalShader->getHandle("uNormalID"), 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(4));
        glUniform1i(directionalShader->getHandle("uSpecularID"), 3);

        glUniform2f(directionalShader->getHandle("uScreenSize"), 600, 600);

        Light * light;
        for (std::list<Light *>::iterator it = lights.begin(); it != lights.end(); it++){
            light = *it;

            glm::vec4 lightDir = CamManager::currentCam()->getViewMatrix() * glm::vec4(light->dir, 0);

            glUniform3f(directionalShader->getHandle("uLightColor"), light->color.x, light->color.y, light->color.z);
            glUniform3f(directionalShader->getHandle("uLightDir"), lightDir.x, lightDir.y, lightDir.z);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }*/

        glUseProgram(directionalShader->getId());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuf);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glEnableVertexAttribArray(directionalShader->getHandle("aPosition"));
        glVertexAttribPointer(directionalShader->getHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

        glUniform3f(directionalShader->getHandle("uEye"),
            CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(0));
        glUniform1i(directionalShader->getHandle("uDiffuseID"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(2));
        glUniform1i(directionalShader->getHandle("uPositionID"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(3));
        glUniform1i(directionalShader->getHandle("uNormalID"), 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(4));
        glUniform1i(directionalShader->getHandle("uSpecularID"), 3);

        glUniform2f(directionalShader->getHandle("uScreenSize"), width, height);

        Light * light;
        for (std::list<Light *>::iterator it = lights.begin(); it != lights.end(); it++){
            light = *it;

            glm::vec4 lightDir = CamManager::currentCam()->getViewMatrix() * glm::vec4(light->dir, 0);

            glUniform3f(directionalShader->getHandle("uLightColor"), light->color.x, light->color.y, light->color.z);
            glUniform3f(directionalShader->getHandle("uLightDir"), lightDir.x, lightDir.y, lightDir.z);

            glUniformMatrix4fv(directionalShader->getHandle("uMVP"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
    }

    void LightProcessor::passAmbientLight(Processor * processor){
        glUseProgram(ambientShader->getId());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuf);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glEnableVertexAttribArray(ambientShader->getHandle("aPosition"));
        glVertexAttribPointer(ambientShader->getHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, processor->getOutFBO()->getTexture(5));
        glUniform1i(ambientShader->getHandle("uAmbientID"), 0);

        glUniform2f(ambientShader->getHandle("uScreenSize"), width, height);

        glUniformMatrix4fv(ambientShader->getHandle("uMVP"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    void LightProcessor::initShader(){
        pointShader = LoadManager::getShader(
            "light-vertex.glsl",
            "light-point-fragment.glsl");

        spotShader = LoadManager::getShader(
            "light-vertex.glsl",
            "light-spot-fragment.glsl");

        directionalShader = LoadManager::getShader(
            "light-vertex.glsl",
            "light-directional-fragment.glsl");

        ambientShader = LoadManager::getShader(
            "light-vertex.glsl",
            "light-ambient-fragment.glsl");
    }

    void LightProcessor::initGeometry(){
        pointLightArea = LoadManager::getMesh("lightarea.obj");

        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, pointLightArea->getVertices().size()*sizeof(float),
            &pointLightArea->getVertices()[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, pointLightArea->getIndices().size()*sizeof(unsigned int),
            &pointLightArea->getIndices()[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
        glClearColor(refreshColor.x, refreshColor.y, refreshColor.z, refreshColor.w);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuf);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glEnableVertexAttribArray(blurShader->getHandle("aPosition"));
        glVertexAttribPointer(blurShader->getHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

        outFBO = processor->getOutFBO();
        int alphaMapTexID = 0;
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

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

    void PostProcessor::passUnaryTextureOp(GLuint tex1, Shader * op){
        glViewport(0, 0, width, height);

        glUseProgram(op->getId());
        glClearColor(refreshColor.x, refreshColor.y, refreshColor.z, refreshColor.w);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuf);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glEnableVertexAttribArray(op->getHandle("aPosition"));
        glVertexAttribPointer(op->getHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

        fbos[0]->enable();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex1);
        glUniform1i(op->getHandle("uTex1ID"), 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        fbos[0]->disable();
        outFBO = fbos[0];

        glDisableVertexAttribArray(op->getHandle("aPosition"));

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }

    void PostProcessor::passBinaryTextureOp(GLuint tex1, GLuint tex2, Shader * op){
        glViewport(0, 0, width, height);

        glUseProgram(op->getId());
        glClearColor(refreshColor.x, refreshColor.y, refreshColor.z, refreshColor.w);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuf);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glEnableVertexAttribArray(op->getHandle("aPosition"));
        glVertexAttribPointer(op->getHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

        fbos[0]->enable();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex1);
        glUniform1i(op->getHandle("uTex1ID"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex2);
        glUniform1i(op->getHandle("uTex2ID"), 1);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        fbos[0]->disable();
        outFBO = fbos[0];

        glDisableVertexAttribArray(op->getHandle("aPosition"));

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }

    void PostProcessor::passTernaryTextureOp(GLuint tex1, GLuint tex2, GLuint tex3, Shader * op){
        glViewport(0, 0, width, height);

        glUseProgram(op->getId());
        glClearColor(refreshColor.x, refreshColor.y, refreshColor.z, refreshColor.w);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuf);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
        glEnableVertexAttribArray(op->getHandle("aPosition"));
        glVertexAttribPointer(op->getHandle("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

        fbos[0]->enable();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex1);
        glUniform1i(op->getHandle("uTex1ID"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex2);
        glUniform1i(op->getHandle("uTex2ID"), 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, tex3);
        glUniform1i(op->getHandle("uTex3ID"), 2);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        fbos[0]->disable();
        outFBO = fbos[0];

        glDisableVertexAttribArray(op->getHandle("aPosition"));

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }
}
