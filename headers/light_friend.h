#ifndef __LIGHT_FRIEND_H__
#define __LIGHT_FRIEND_H__

#include "GLSL.h"

#include "mesh.h"
#include "shader.h"
#include "object3d.h"
#include "load_manager.h"
#include "material.h"
#include "virtual_camera.h"
#include "log.h"
#include "light.h"
#include "collision_manager.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "game_map.h"
#include "light_shot.h"

class LightFriend : public Object3D{
public:
    LightFriend(GameMap * _map, int cubeIndex) : gameMap(_map), currentCube(cubeIndex){
    }

    void init(){
        mesh = LoadManager::getMesh("sphere-tex.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader(
            "geometry-pass-map-vertex.glsl",
            "geometry-pass-map-fragment.glsl");

        pos = gameMap->getCubePos(currentCube);
        pos.y += defaultHeight;
        initialPos = pos;
        
        light = new Light(
            glm::vec3(rand_float(), rand_float(), rand_float()), 
            pos, glm::vec3(0, -1, 0), glm::vec3(0, minPower, 0), 45.0f);
        LightManager::addLight(light);
    }

    void drawObject(){
        if (free)
            updateFloatMovement();
        light->pos = glm::vec3(pos.x, pos.y + 3, pos.z);

        loadIdentity();

        addTransformation(glm::translate(glm::mat4(1.0f), pos));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.5)));

        glUseProgram(shader->getId());

        enableAttrArray3f("aPosition", "posBufObj");
        enableAttrArray3f("aNormal", "norBufObj");
        bindElements();

        Material::SetMaterial(Material::GOLD, shader);
        glUniform3f(shader->getHandle("UeColor"), light->color.r, light->color.g, light->color.b);

        glUniform1i(shader->getHandle("uCompleteGlow"), 1);

        bindUniformMatrix4f(
            shader->getHandle("uProjMatrix"),
            CamManager::currentCam()->projectionMatrix);

        bindUniformMatrix4f(
            shader->getHandle("uViewMatrix"),
            CamManager::currentCam()->getViewMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        drawElements();

        disableAttrArray("aPosition");
        disableAttrArray("aNormal");
        unbindAll();

    }

    float rand_float(){
        return ((float)rand()) / ((float)RAND_MAX);
    }


    void onCollision(Object3D * obj){
    }

    void lightUp(){
        light->fallOff.y = maxPower;
        free = true;
    }

private:
    static const int UP_DIR = 1;
    static const int DOWN_DIR = -1;

    glm::vec3 pos, initialPos;
    int currentCube;

    float maxPower = 0.03f, minPower = 0.3f;
    bool free = false;

    int floatDirection = UP_DIR;
    float defaultHeight = 2, heightOffset = 0.4, floatVeloctiy = 0.008f;

 
    GameMap * gameMap;
    Light * light;

    void updateFloatMovement(){
        pos.y += floatDirection*floatVeloctiy;

        if (pos.y >= initialPos.y + heightOffset){
            floatDirection = DOWN_DIR;
        }
        else if (pos.y <= initialPos.y - heightOffset){
            floatDirection = UP_DIR;
        }
    }
};


#endif