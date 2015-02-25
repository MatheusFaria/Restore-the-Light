#ifndef __SCENE_H__
#define __SCENE_H__

#include "mesh.h"
#include "shader.h"
#include "object3d.h"
#include "load_manager.h"
#include "material.h"
#include "virtual_camera.h"
#include "log.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <string>
#include <stack>

using namespace std;

glm::vec3 g_light(0.5, 0.5, 5);

class Cube : public Object3D{
public:
    Cube(){}

    void init(){
        mesh = LoadManager::getMesh("cube.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader("vert.glsl", "frag.glsl");
        shader->loadHandle("aPosition");
        shader->loadHandle("aNormal");
        shader->loadHandle("uProjMatrix");
        shader->loadHandle("uViewMatrix");
        shader->loadHandle("uModelMatrix");
        shader->loadHandle("uLightPos");
        shader->loadHandle("UaColor");
        shader->loadHandle("UdColor");
        shader->loadHandle("UsColor");
        shader->loadHandle("UeColor");
        shader->loadHandle("Ushine");
        shader->loadHandle("uEye");
    }

    void drawObject(){
        glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);
        glUniform3f(shader->getHandle("uLightPos"), g_light.x, g_light.y, g_light.z);
        glUniform3f(shader->getHandle("uEye"), CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);

        Material::SetMaterial(Material::BLUE_PLASTIC, shader);
        
        bindModelMatrix("uModelMatrix");

        drawElements();
    }
};

class GameMap : public Object3D{
public:
    string mapLayout = "c.c..c.cc.c..c.cc.c.";
    int mapRowsSize = 4;
    int mapColsSize = 5;

    GameMap(){
        if (mapLayout.size() != mapRowsSize*mapColsSize){
            Log::error("GameMap::generateMap", "Incorrect size for a map", "", mapLayout);
        }
    }

    void init(){
        for (int index = 0; index < mapRowsSize*mapColsSize; index++){
            Cube * c = new Cube();
            c->init();
            addChild(c);
            switch (mapLayout[index]){
            case DEFAULT_CUBE:
                c->addTransformation(glm::translate(glm::mat4(1.0f), getCubePos(index)));
                break;
            case EMPTY_SPACE:
            default:
                break;
            }
        }

        addTransformation(glm::rotate(glm::mat4(1.0f), 30.0f, glm::vec3(0, 0, 1)));
    }

    void drawObject(){
    }

    glm::vec3 getCubePos(int cubePos){
        return glm::vec3(
            -CUBE_SIDE*(cubePos % mapRowsSize), 
            0.2*((cubePos % 2) == 0 ? -1 : 1), 
            -CUBE_SIDE*(cubePos / mapRowsSize));
    }

private:
    static const int CUBE_SIDE = 2;
    static const char EMPTY_SPACE = '.';
    static const char DEFAULT_CUBE = 'c';
};


class Hero : public Object3D{
public:
    Hero(GameMap * _map): gameMap(_map){}

    void init(){
        mesh = LoadManager::getMesh("sphere.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader("vert.glsl", "frag.glsl");
        shader->loadHandle("aPosition");
        shader->loadHandle("aNormal");
        shader->loadHandle("uProjMatrix");
        shader->loadHandle("uViewMatrix");
        shader->loadHandle("uModelMatrix");
        shader->loadHandle("uLightPos");
        shader->loadHandle("UaColor");
        shader->loadHandle("UdColor");
        shader->loadHandle("UsColor");
        shader->loadHandle("UeColor");
        shader->loadHandle("Ushine");
        shader->loadHandle("uEye");
    }

    void drawObject(){
        glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);
        glUniform3f(shader->getHandle("uLightPos"), g_light.x, g_light.y, g_light.z);
        glUniform3f(shader->getHandle("uEye"), CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);

        Material::SetMaterial(Material::GOLD, shader);
        loadIdentity();

        glm::vec3 pos = gameMap->getCubePos(currentCube);
        pos.y += 3;

        addTransformation(glm::translate(glm::mat4(1.0f), pos));
        bindModelMatrix("uModelMatrix");

        drawElements();
    }


    int currentCube = 0;
    GameMap * gameMap;
};


class Enemy : public Object3D{
public:
    Enemy(GameMap * _map) : gameMap(_map){}

    void init(){
        mesh = LoadManager::getMesh("cube.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader("vert.glsl", "frag.glsl");
        shader->loadHandle("aPosition");
        shader->loadHandle("aNormal");
        shader->loadHandle("uProjMatrix");
        shader->loadHandle("uViewMatrix");
        shader->loadHandle("uModelMatrix");
        shader->loadHandle("uLightPos");
        shader->loadHandle("UaColor");
        shader->loadHandle("UdColor");
        shader->loadHandle("UsColor");
        shader->loadHandle("UeColor");
        shader->loadHandle("Ushine");
        shader->loadHandle("uEye");
    }

    void drawObject(){
        glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);
        glUniform3f(shader->getHandle("uLightPos"), g_light.x, g_light.y, g_light.z);
        glUniform3f(shader->getHandle("uEye"), CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);

        stack<glm::mat4> matrixStack;
        matrixStack.push(glm::mat4(1.0f));

        Material::SetMaterial(Material::FLAT_GREY, shader);
        loadIdentity();
        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)));
        bindModelMatrix("uModelMatrix");

        drawElements();
    }


    int currentCube = 0;
    GameMap * gameMap;
};


#endif