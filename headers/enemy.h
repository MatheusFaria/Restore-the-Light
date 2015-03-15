#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "object3d.h"
#include "game_map.h"
#include "collision_manager.h"

class BodyPart : public Object3D{
public:
    BodyPart(Mesh * m, Shader * s, Object3D * _body) : body(_body){
        mesh = m;
        shader = s;
    }

    void init(){
        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadTextureBuffer("texBufObj");
        loadElementBuffer();

        collisionListPos = CollisionManager::addEnemy(this);
        setCollisionsMask(4, 0);
    }

    void drawObject(){

    }

    void setMatrix(glm::mat4 matrix){
        loadIdentity();
        addTransformation(matrix);
    }

    void onCollision(Object3D * obj){
        body->onCollision(obj);
    }

    void kill(){
        std::cout << mesh->getFileName() << std::endl;
        CollisionManager::removeEnemy(collisionListPos);
    }

private:
    Object3D * body;
    std::list<Object3D *>::iterator collisionListPos;
};


class Enemy: public Object3D{
public:
    Enemy(GameMap * _gameMap, int _cubePos) : gameMap(_gameMap), currentCube(_cubePos){}

    void init(){
        mesh = LoadManager::getMesh("cube-textures.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadTextureBuffer("texBufObj");
        loadElementBuffer();

        pos = gameMap->getCubePos(currentCube);
        pos.y += 3;

        shader = LoadManager::getShader(
            "geometry-pass-map-texture-vertex.glsl",
            "geometry-pass-map-texture-fragment.glsl");

        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/head.obj") , shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/torso.obj"), shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/leftarm.obj"), shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/rightarm.obj"), shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/lefteyelid.obj"), shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/righteyelid.obj"), shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/leftthigh.obj"), shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/rightthigh.obj"), shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/leftfoot.obj"), shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/rightfoot.obj"), shader, this));


        for (int i = 0; i < parts.size(); i++){
            parts[i]->init();
        }
        x = -0.19;
        y = -0.74;
        z = 0.27;

        tex = TextureManager::getTexture("enemy-shot.bmp")->getTexture();
        atex = TextureManager::getTexture("enemy-alpha.bmp")->getTexture();
    }

    void drawObject(){
        if (finished)
            return;

        glUseProgram(shader->getId());

        Material::SetMaterial(Material::BLUE_PLASTIC, shader, false, false);
        glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);

        glUniform1i(shader->getHandle("uCompleteGlow"), 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(shader->getHandle("uTexID"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, atex);
        glUniform1i(shader->getHandle("uAlphaTexID"), 1);


        bindUniformMatrix4f(
            shader->getHandle("uProjMatrix"),
            CamManager::currentCam()->projectionMatrix);

        bindUniformMatrix4f(
            shader->getHandle("uViewMatrix"),
            CamManager::currentCam()->getViewMatrix());

        loadIdentity();


        //Head
        enablePartArrays(head);

        addTransformation(glm::translate(glm::mat4(1.0f), pos));
        pushMatrix();
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[head]->setMatrix(getModelMatrix());
        
        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));
        
        parts[head]->drawElements();

        reloadTopMatrix();

        //Left Eye lid
        enablePartArrays(lefteyelid);

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0.17, 0.19, 0.23)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[lefteyelid]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[lefteyelid]->drawElements();

        reloadTopMatrix();


        //Right Eye lid
        enablePartArrays(righteyelid);
        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(-0.23, 0.17, 0.23)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[righteyelid]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[righteyelid]->drawElements();

        reloadTopMatrix();

        //Torso
        enablePartArrays(torso);

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(-0.02, -0.581, -0.15)));
        pushMatrix();
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[torso]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[torso]->drawElements();

        reloadTopMatrix();

        //Left Arm
        enablePartArrays(leftarm);

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0.83, -0.12, -0.11)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[leftarm]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[leftarm]->drawElements();

        reloadTopMatrix();

        //Right Arm
        enablePartArrays(rightarm);

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(-0.69, -0.08, -0.04)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[rightarm]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[rightarm]->drawElements();

        reloadTopMatrix();


        //Left Thigh
        enablePartArrays(leftthigh);

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0.35, -0.61, 0)));
        pushMatrix();
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[leftthigh]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[leftthigh]->drawElements();

        reloadTopMatrix();

        //Left Foot
        enablePartArrays(leftfoot);

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0.19, -0.74, 0.27)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[leftfoot]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[leftfoot]->drawElements();

        reloadTopMatrix();


        //Right Thigh
        enablePartArrays(rightthigh);

        popMatrix();
        reloadTopMatrix();

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(-0.32, -0.62, -0.03)));
        pushMatrix();
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[rightthigh]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[rightthigh]->drawElements();

        reloadTopMatrix();

        //Right Foot
        enablePartArrays(rightfoot);

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(-0.19, -0.74, 0.27)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[rightfoot]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[rightfoot]->drawElements();

        reloadTopMatrix();


        disableAttrArray("aPosition");
        disableAttrArray("aNormal");
        disableAttrArray("aTexCoord");
        unbindAll();

        update();
    }

    void update(){

    }

    void onCollision(Object3D *obj){
        if (finished)
            return;

        if (obj->getCollisionTypeMask() == 3){
            for (int i = 0; i < parts.size(); i++){
                parts[i]->kill();
            }
            if (parent != NULL)
                parent->removeChild(this);
            finished = true;
        }
    }

    void checkInput(GLFWwindow * window){

        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
            z += 0.01;
            std::cout << x << " " << y << " " << z << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
            z -= 0.01;
            std::cout << x << " " << y << " " << z << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
            y += 0.01;
            std::cout << x << " " << y << " " << z << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){
            y -= 0.01;
            std::cout << x << " " << y << " " << z << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS){
            x += 0.01;
            std::cout << x << " " << y << " " << z << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
            x -= 0.01;
            std::cout << x << " " << y << " " << z << std::endl;
        }
    }

private:
    float z, y, x;

    GameMap * gameMap;
    int currentCube;
    std::vector<BodyPart *> parts;
    bool finished = false;

    glm::vec3 pos;

    GLuint tex, atex;

    const int head = 0, torso = 1, leftarm = 2, rightarm = 3,
        lefteyelid = 4, righteyelid = 5, leftthigh = 6, rightthigh = 7,
        leftfoot = 8, rightfoot = 9;


    void enablePartArrays(int part){
        parts[part]->enableAttrArray3f("aPosition", "posBufObj");
        parts[part]->enableAttrArray3f("aNormal", "norBufObj");
        parts[part]->enableAttrArray3f("aTexCoord", "texBufObj");
        parts[part]->bindElements();
    }

};

#endif