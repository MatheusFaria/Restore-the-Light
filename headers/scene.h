#ifndef __SCENE_H__
#define __SCENE_H__
#include <fstream>

typedef unsigned char Uint8;

#include "mesh.h"
#include "shader.h"
#include "object3d.h"
#include "load_manager.h"
#include "material.h"
#include "virtual_camera.h"
#include "log.h"
#include "light.h"
#include "GLSL.h"

#include "image.h"
#include "texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <string>
#include <stack>

using namespace std;

class GameMap : public Object3D{
public:
    string mapLayout;
    int mapRowsSize;
    int mapColsSize;
    GLuint tex, atex;
    vector<glm::mat4> cubesMatrices;

    GameMap(string _mapLayout, int rows, int cols):
    mapLayout(_mapLayout), mapRowsSize(rows), mapColsSize(cols){
        if (mapLayout.size() != mapRowsSize*mapColsSize){
            Log::error("GameMap::generateMap", "Incorrect size for a map", "", mapLayout);
        }
    }

    void init(){
        mesh = LoadManager::getMesh("cube-textures.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadTextureBuffer("texBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader("vert-map.glsl", "frag-map.glsl");

        tex = TextureManager::getTexture("enemy.bmp")->getTexture();
        atex = TextureManager::getTexture("enemy-alpha.bmp")->getTexture();

        for (int index = 0; index < mapRowsSize*mapColsSize; index++){
            switch (mapLayout[index]){
            case DEFAULT_CUBE:
                cubesMatrices.push_back(glm::translate(glm::mat4(1.0f), getCubePos(index)));
                break;
            case EMPTY_SPACE:
            default:
                break;
            }
        }
    }

    void drawObject(){
        Material::SetMaterial(Material::SILVER, shader, false);
        glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);

        enableAttrArray2f("aTexCoord", "texBufObj");

        glUniform1i(shader->getHandle("uCompleteGlow"), 0);
        glUniform1i(shader->getHandle("uApplyTexture"), 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(shader->getHandle("uTexID"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, atex);
        glUniform1i(shader->getHandle("uAlphaTexID"), 1);
        
        
        for (int index = 0; index < cubesMatrices.size(); index++){
            addTransformation(cubesMatrices[index]);
            bindModelMatrix("uModelMatrix");
            drawElements();
            reloadTopMatrix();
        }
    }

    glm::vec3 getCubePos(int cubePos){
        int i = cubePos / mapColsSize;
        int j = cubePos % mapColsSize;

        float x = -CUBE_SIDE*i;
        float z = -CUBE_SIDE*j;
        float y = 0.2;
        if ((i + j) % 2)
            y *= -1;

        return glm::vec3(x, y, z);
    }

    bool thereIsCube(int pos){
        if (pos < mapLayout.size() && pos >= 0){
            return mapLayout[pos] == DEFAULT_CUBE;
        }
        return false;
    }

    bool thereIsCube(glm::vec3 pos){
        pos = -1.0f * pos;
        float x1 = pos.x / CUBE_SIDE, y1 = pos.y, 
            z1 = pos.z/CUBE_SIDE;

        int x = (int) x1, z = (int) z1;

        if (pos.x <= x*CUBE_SIDE - 1){
            x--;
        }
        else if (pos.x > x*CUBE_SIDE + 1){
            x++;
        }

        if (pos.z <= z*CUBE_SIDE - 1){
            z--;
        }
        else if (pos.z > z*CUBE_SIDE + 1){
            z++;
        }

        if (x >= 0 && z >= 0 && x < mapRowsSize && z < mapColsSize){
            return thereIsCube(getMapPos(x, z));
        }
        return false;
    }

    int getMapPos(int i, int j){
        return i*mapColsSize + j;
    }

private:
    static const int CUBE_SIDE = 2;
    static const char EMPTY_SPACE = '.';
    static const char DEFAULT_CUBE = 'c';
};

class Item : public Object3D{
public:
    Item(GameMap * _map, int cubeIndex, int rot) : gameMap(_map), currentCube(cubeIndex){
        rotAxe = (float) rot;
    }

    void init(){
        mesh = LoadManager::getMesh("bunny.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader("vert.glsl", "frag.glsl");
    }

    void drawObject(){
        glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);
        glUniform3f(shader->getHandle("uEye"), CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);

        Material::SetMaterial(Material::EMERALD, shader);
        loadIdentity();

        glm::vec3 pos = gameMap->getCubePos(currentCube);
        pos.y += 1.6;

        addTransformation(glm::translate(glm::mat4(1.0f), pos));
        addTransformation(glm::rotate(glm::mat4(1.0f), rotAxe, glm::vec3(0, 1, 0)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.6)));
        bindModelMatrix("uModelMatrix");

        drawElements();
    }

    float rotAxe;
    int currentCube;
    GameMap * gameMap;
};

class Enemy : public Object3D{
public:
    Enemy(GameMap * _map, int cubeIndex, int rot) : gameMap(_map), currentCube(cubeIndex){
        arms_angle = 0;
        direction = -1;
        rotAxe = (float)rot;
    }

    void init(){
        mesh = LoadManager::getMesh("cube.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader("vert.glsl", "frag.glsl");
    }

    void drawObject(){
        glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);
        glUniform3f(shader->getHandle("uEye"), CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);

        Material::SetMaterial(Material::OBSIDIAN, shader);
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
        addTransformation(glm::rotate(glm::mat4(1.0f), rotAxe, glm::vec3(0, 1, 0)));
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
    float rotAxe;

    void armsAnimationUpdate(){
        arms_angle += direction*5.0f;

        if (arms_angle <= -90.0f ||
            arms_angle >= 0.0f){
            direction *= -1;
        }
    }
};

#endif