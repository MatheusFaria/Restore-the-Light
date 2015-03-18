#ifndef __GAME_MAP_H__
#define __GAME_MAP_H__

#include <iostream>
#include <string>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_PURE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "object3d.h"
#include "load_manager.h"
#include "material.h"
#include "virtual_camera.h"
#include "collision_manager.h"

using namespace std;

class GroundPiece : public Object3D{
public:
    GroundPiece(glm::mat4 _translateMat) : translateMat(_translateMat){}

    void init(){
        mesh = LoadManager::getMesh("cube-textures.obj");

        addTransformation(translateMat);

        CollisionManager::addGround(this);
        setCollisionsMask(1, 0);
    }

    void drawObject(){}
    void onCollision(Object3D * obj){}

private:
    glm::mat4 translateMat;
};

class GameMap : public Object3D{
public:
    GameMap(string _mapLayout, int _rows, int _cols):
    mapLayout(_mapLayout), rows(_rows), cols(_cols){
        if (mapLayout.size() != rows*cols){
            std::cout << "GameMap::generateMap: " << "Incorrect size for a map\n" << mapLayout << "\n";
        }
    }

    void init(){
        mesh = LoadManager::getMesh("cube-textures.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader(
            "geometry-pass-map-vertex.glsl",
            "geometry-pass-map-fragment.glsl");

        GroundPiece * piece;
        for (int index = 0; index < rows*cols; index++){
            switch (mapLayout[index]){
            case DEFAULT_CUBE:
                piece = new GroundPiece(glm::translate(glm::mat4(1.0f), getCubePos(index)));
                piece->init();

                cubesMatrices.push_back(glm::translate(glm::mat4(1.0f), getCubePos(index)));

                cubesIndices.push_back(index);
                break;
            case EMPTY_SPACE:
            default:
                break;
            }
            lightPath.push_back(0);
        }
    }

    void drawObject(){
        glUseProgram(shader->getId());

        enableAttrArray3f("aPosition", "posBufObj");
        enableAttrArray3f("aNormal", "norBufObj");
        bindElements();

        Material::SetMaterial(Material::FLAT_GREY, shader, false);
        
        bindUniformMatrix4f(
            shader->getHandle("uProjMatrix"),
            CamManager::currentCam()->projectionMatrix);

        bindUniformMatrix4f(
            shader->getHandle("uViewMatrix"),
            CamManager::currentCam()->getViewMatrix());


        for (int index = 0; index < cubesMatrices.size(); index++){
            addTransformation(cubesMatrices[index]);

            glUniform1i(shader->getHandle("uCompleteGlow"), lightPath[cubesIndices[index]]);

            if (lightPath[cubesIndices[index]] == 1){
                glUniform3f(shader->getHandle("UeColor"), 0.6, 0.6, 0);
            }
            else{
                glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);
            }

            bindUniformMatrix4f(
                shader->getHandle("uModelMatrix"),
                getModelMatrix());

            bindUniformMatrix4f(
                shader->getHandle("uNormalMatrix"),
                glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

            drawElements();
            reloadTopMatrix();
        }

        disableAttrArray("aPosition");
        disableAttrArray("aNormal");
        unbindAll();
    }

    glm::vec3 getCubePos(int pos){
        int i = pos / cols;
        int j = pos % cols;

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

    bool thereIsCube(int i, int j){
        return thereIsCube(getMapPos(i, j));
    }

    bool thereIsCube(glm::vec3 pos){
        return thereIsCube(getCubePos(pos));
    }

    int getMapPos(int i, int j){
        return i*cols + j;
    }

    int getCubePos(glm::vec3 pos){
        pos = -1.0f * pos;
        float x1 = pos.x / CUBE_SIDE, y1 = pos.y,
            z1 = pos.z / CUBE_SIDE;

        int x = (int)x1, z = (int)z1;

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

        if (x >= 0 && z >= 0 && x < rows && z < cols){
            return getMapPos(x, z);
        }
        return -1;
    }

    void onCollision(Object3D * obj){}

    void litCube(int cube){
        lightPath[cube] = 1;
    }

    void lightAllTheWay(){
        for (int i = 0; i < lightPath.size(); i++){
            lightPath[i] = 1;
        }
    }

    void clearPath(){
        for (int i = 0; i < lightPath.size(); i++){
            lightPath[i] = 0;
        }
    }

private:
    static const int CUBE_SIDE = 2;
    static const char EMPTY_SPACE = '.';
    static const char DEFAULT_CUBE = 'c';

    string mapLayout;
    int rows, cols;

    vector<glm::mat4> cubesMatrices;
    vector<int> lightPath;
    vector<int> cubesIndices;
};

#endif