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

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <string>
#include <stack>

using namespace std;

glm::vec3 g_light(-6, 5, -6);

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
        mesh = LoadManager::getMesh("cube.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader("vert.glsl", "frag.glsl");

        //loadIdentity();
        //addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0, -2, 0)));
        //addTransformation(glm::rotate(glm::mat4(1.0f), 30.0f, glm::vec3(0, 0, 1)));
        pushMatrix();
    }

    void drawObject(){
        glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);
        glUniform3f(shader->getHandle("uEye"), 
            CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);
        LightManager::loadLights(shader->getHandle("uLightPos"),
            shader->getHandle("uLightColor"),
            shader->getHandle("uLightFallOff"));

        Material::SetMaterial(Material::SILVER, shader);
        
        
        for (int index = 0; index < mapRowsSize*mapColsSize; index++){
            switch (mapLayout[index]){
            case DEFAULT_CUBE:
                addTransformation(glm::translate(glm::mat4(1.0f), getCubePos(index)));
                break;
            case EMPTY_SPACE:
            default:
                break;
            }

            bindModelMatrix("uModelMatrix");
            drawElements();
            reloadTopMatrix();
        }
    }

    glm::vec3 getCubePos(int cubePos){
        float x = -CUBE_SIDE*(cubePos % mapRowsSize);
        float z = -CUBE_SIDE*(cubePos / mapRowsSize);
        float y = 0.2;
        if (((int)(cubePos % mapRowsSize + cubePos / mapRowsSize)) % 2)
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
            return thereIsCube(x*mapRowsSize + z);
        }
        return false;
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
        LightManager::loadLights(shader->getHandle("uLightPos"),
            shader->getHandle("uLightColor"),
            shader->getHandle("uLightFallOff"));

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

class LightObject : public Object3D{
public:
    LightObject(){}

    void init(){
        mesh = LoadManager::getMesh("sphere.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader("vert.glsl", "frag.glsl");
    }

    void drawObject(){
        glUniform3f(shader->getHandle("UeColor"), 1, 1, 1);
        glUniform3f(shader->getHandle("uEye"), CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);
        LightManager::loadLights(shader->getHandle("uLightPos"),
            shader->getHandle("uLightColor"),
            shader->getHandle("uLightFallOff"));

        Material::SetMaterial(Material::EMERALD, shader);
        loadIdentity();

        addTransformation(glm::translate(glm::mat4(1.0f), g_light));
        bindModelMatrix("uModelMatrix");

        drawElements();
    }
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
        LightManager::loadLights(shader->getHandle("uLightPos"),
            shader->getHandle("uLightColor"),
            shader->getHandle("uLightFallOff"));

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

class Cube : public Object3D{
public:
    Cube(){}

    void init(){
        mesh = LoadManager::getMesh("sphere-tex.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadTextureBuffer("texBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader("vert.glsl", "frag.glsl");

        LoadManager::getImage("bloomtex.bmp");
        textureLoad();
    }

    void drawObject(){
        enableAttrArray2f("aTexCoord", "texBufObj");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(shader->getHandle("uTextureID"), 0);
        
        glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);
        glUniform3f(shader->getHandle("uEye"), CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);
        LightManager::loadLights(shader->getHandle("uLightPos"),
            shader->getHandle("uLightColor"),
            shader->getHandle("uLightFallOff"));

        Material::SetMaterial(Material::EMERALD, shader);
        loadIdentity();

        rot += 0.05f;
        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5)));
        addTransformation(glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0, 1, 0)));
        bindModelMatrix("uModelMatrix");

        drawElements();
    }

    void textureLoad(){
        glGenTextures(1, &texture);             // Generate a texture
        glBindTexture(GL_TEXTURE_2D, texture); // Bind that texture temporarily

        GLint mode = GL_RGB;                   // Set the mode

        // Create the texture. We get the offsets from the image, then we use it with the image's
        // pixel data to create it.
        glTexImage2D(GL_TEXTURE_2D, 0, mode, LoadManager::getImage("bloomtex.bmp")->width, 
            LoadManager::getImage("bloomtex.bmp")->height, 0, mode, GL_UNSIGNED_BYTE, 
            LoadManager::getImage("bloomtex.bmp")->pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Unbind the texture
        glBindTexture(GL_TEXTURE_2D, NULL);
    }
    float rot = 0;
    GLuint texture;

};

#endif