#ifndef __GAME_MAP_H__
#define __GAME_MAP_H__

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
#include "collision_manager.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <string>

using namespace std;

class GroundPiece : public Object3D{
public:
    GroundPiece(glm::mat4 _translateMat) : translateMat(_translateMat){

    }

    void init(){
        mesh = LoadManager::getMesh("cube-textures.obj");

        addTransformation(translateMat);

        CollisionManager::addGround(this);
    }

    void drawObject(){

    }
    void onCollision(Object3D * obj){
    }

private:
    glm::mat4 translateMat;
};

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

        shader = LoadManager::getShader(
            "geometry-pass-map-texture-vertex.glsl",
            "geometry-pass-map-texture-fragment.glsl");

        tex = TextureManager::getTexture("enemy.bmp")->getTexture();
        atex = TextureManager::getTexture("enemy-alpha.bmp")->getTexture();


        GroundPiece * piece;
        for (int index = 0; index < mapRowsSize*mapColsSize; index++){
            switch (mapLayout[index]){
            case DEFAULT_CUBE:
                piece = new GroundPiece(glm::translate(glm::mat4(1.0f), getCubePos(index)));
                piece->init();
                cubesMatrices.push_back(glm::translate(glm::mat4(1.0f), getCubePos(index)));
                break;
            case EMPTY_SPACE:
            default:
                break;
            }
        }
    }

    void drawObject(){
        glUseProgram(shader->getId());

        enableAttrArray3f("aPosition", "posBufObj");
        enableAttrArray3f("aNormal", "norBufObj");
        enableAttrArray2f("aTexCoord", "texBufObj");
        bindElements();

        Material::SetMaterial(Material::FLAT_GREY, shader, false, false);
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


        for (int index = 0; index < cubesMatrices.size(); index++){
            addTransformation(cubesMatrices[index]);

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
        disableAttrArray("aTexCoord");
        unbindAll();
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

    void onCollision(Object3D * obj){
    }

private:
    static const int CUBE_SIDE = 2;
    static const char EMPTY_SPACE = '.';
    static const char DEFAULT_CUBE = 'c';
};

#endif