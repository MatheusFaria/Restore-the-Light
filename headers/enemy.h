#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "object3d.h"
#include "game_map.h"
#include "collision_manager.h"

class BodyPart : public Object3D{
public:
    BodyPart(Mesh * m, Object3D * _body) : body(_body){
        mesh = m;
    }

    void init(){
        collisionListPos = CollisionManager::addEnemy(this);
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
        //loadTextureBuffer("texBufObj");
        loadElementBuffer();

        pos = gameMap->getCubePos(currentCube);
        pos.y += 2;

        shader = LoadManager::getShader(
            "geometry-pass-map-vertex.glsl",
            "geometry-pass-map-fragment.glsl");

        BodyPart * b = new BodyPart(mesh, this);
        b->init();
        parts.push_back(b);

        //tex = TextureManager::getTexture("enemy.bmp")->getTexture();
        //atex = TextureManager::getTexture("enemy-alpha.bmp")->getTexture();
    }

    void drawObject(){
        if (finished)
            return;

        glUseProgram(shader->getId());

        enableAttrArray3f("aPosition", "posBufObj");
        enableAttrArray3f("aNormal", "norBufObj");
        bindElements();

        Material::SetMaterial(Material::BLUE_PLASTIC, shader);
        glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);

        glUniform1i(shader->getHandle("uCompleteGlow"), 0);

        loadIdentity();

        addTransformation(glm::translate(glm::mat4(1.0f), pos));

        parts[0]->setMatrix(getModelMatrix());

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

        update();
    }

    void update(){

    }

    void onCollision(Object3D *obj){
        for (int i = 0; i < parts.size(); i++){
            parts[i]->kill();
        }
        if (parent != NULL)
            parent->removeChild(this);
        finished = true;
    }

private:
    GameMap * gameMap;
    int currentCube;
    std::vector<BodyPart *> parts;
    bool finished = false;

    glm::vec3 pos;

};

#endif