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
    string mapLayout;
    int mapRowsSize;
    int mapColsSize;

    GameMap(string _mapLayout, int rows, int cols):
    mapLayout(_mapLayout), mapRowsSize(rows), mapColsSize(cols){
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

        //addTransformation(glm::rotate(glm::mat4(1.0f), 30.0f, glm::vec3(0, 0, 1)));
    }

    void drawObject(){
    }

    glm::vec3 getCubePos(int cubePos){
        float x = -CUBE_SIDE*(cubePos % mapRowsSize);
        float z = -CUBE_SIDE*(cubePos / mapRowsSize);
        float y = 0.2;
        if (((int)(cubePos % mapRowsSize + cubePos / mapRowsSize)) % 2)
            y *= -1;

        return glm::vec3(x, y, z);
    }

private:
    static const int CUBE_SIDE = 2;
    static const char EMPTY_SPACE = '.';
    static const char DEFAULT_CUBE = 'c';
};


class Hero : public Object3D{
public:
    Hero(GameMap * _map, int cubeIndex) : gameMap(_map), currentCube(cubeIndex){
    }

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
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.5)));
        bindModelMatrix("uModelMatrix");

        drawElements();
    }


    int currentCube;
    GameMap * gameMap;
};

class Item : public Object3D{
public:
    Item(GameMap * _map, int cubeIndex) : gameMap(_map), currentCube(cubeIndex){
    }

    void init(){
        mesh = LoadManager::getMesh("bunny.obj");

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
        pos.y += 1.6;

        addTransformation(glm::translate(glm::mat4(1.0f), pos));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.6)));
        bindModelMatrix("uModelMatrix");

        drawElements();
    }


    int currentCube;
    GameMap * gameMap;
};


class Enemy : public Object3D{
public:
    Enemy(GameMap * _map, int cubeIndex) : gameMap(_map), currentCube(cubeIndex){
        arms_angle = 0;
        direction = -1;
    }

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

        Material::SetMaterial(Material::FLAT_GREY, shader);
        glm::vec3 pos = gameMap->getCubePos(currentCube);

        loadIdentity();

        //Cube Base
        /*addTransformation(glm::rotate(glm::mat4(1.0f), base_rot.y, glm::vec3(0, 1, 0)));
        addTransformation(glm::rotate(glm::mat4(1.0f), base_rot.x, glm::vec3(1, 0, 0)));
        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y - 2, pos.z)));
        pushMatrix();

        bindModelMatrix("uModelMatrix");
        drawElements();*/
        
        //tail
        reloadTopMatrix();

        pos.y += 2;
        addTransformation(glm::translate(glm::mat4(1.0f), pos));
        pushMatrix();

        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        bindModelMatrix("uModelMatrix");
        drawElements();

        //hip
        reloadTopMatrix();

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.2 * 2, 0.2)));
        pushMatrix();

        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.4, 0.3, 0.3)));
        bindModelMatrix("uModelMatrix");
        drawElements();

        //chest
        reloadTopMatrix();

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.3 * 2, 0.2)));
        pushMatrix();

        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.65, 0.4, 0.4)));
        bindModelMatrix("uModelMatrix");
        drawElements();

        //left arm
        reloadTopMatrix();

        armsAnimationUpdate();


        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(-0.87, 0.2, 0)));
        addTransformation(glm::rotate(glm::mat4(1.0f), arms_angle, glm::vec3(1, 0, 0)));
        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.4, 0)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.5, 0.2)));
        bindModelMatrix("uModelMatrix");
        drawElements();

        //right arm
        reloadTopMatrix();

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0.87, 0.2, 0)));
        addTransformation(glm::rotate(glm::mat4(1.0f), arms_angle, glm::vec3(1, 0, 0)));
        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.4, 0)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.5, 0.2)));
        bindModelMatrix("uModelMatrix");
        drawElements();

        //head
        reloadTopMatrix();

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.6, 0)));
        pushMatrix();

        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.3, 0.2, 0.2)));
        bindModelMatrix("uModelMatrix");
        drawElements();

        //left eye
        reloadTopMatrix();
        Material::SetMaterial(Material::BLUE_PLASTIC, shader);

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(-0.1, 0, 0.24)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.05)));
        bindModelMatrix("uModelMatrix");
        drawElements();

        //Right eye
        reloadTopMatrix();
        Material::SetMaterial(Material::BLUE_PLASTIC, shader);

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0.1, 0, 0.24)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.05)));
        bindModelMatrix("uModelMatrix");
        drawElements();

        resetMatrix();
    }


    int currentCube;
    GameMap * gameMap;
private:
    float arms_angle;
    int direction;

    void armsAnimationUpdate(){
        arms_angle += direction*5.0f;

        if (arms_angle <= -90.0f ||
            arms_angle >= 0.0f){
            direction *= -1;
        }
    }
};


#endif