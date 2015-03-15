#ifndef LIGHT_SHOT_H
#define LIGHT_SHOT_H

#include "object3d.h"
#include "load_manager.h"
#include "light.h"
#include "material.h"
#include "virtual_camera.h"
#include "collision_manager.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


class LightShot : public Object3D{
public:
    LightShot(glm::vec3 _pos, glm::vec3 _dir, float _velocity, float _power)
        : pos(_pos), dir(_dir), velocity(_velocity), power(_power){
        initialPos = _pos;
    }

    void init(){
        mesh = LoadManager::getMesh("cube-textures.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader(
            "geometry-pass-map-vertex.glsl",
            "geometry-pass-map-fragment.glsl");

        collisionListPos = CollisionManager::addShot(this);

        light = new Light(glm::vec3(1), pos, glm::vec3(0, 0.3, 0));
        LightManager::addLight(light);
        setCollisionsMask(3, 0);
    }

    void drawObject(){
        if (finished)
            return;

        glUseProgram(shader->getId());

        enableAttrArray3f("aPosition", "posBufObj");
        enableAttrArray3f("aNormal", "norBufObj");
        bindElements();

        Material::SetMaterial(Material::SILVER, shader);
        glUniform3f(shader->getHandle("UeColor"), 1, 1, 1);
        glUniform1i(shader->getHandle("uCompleteGlow"), 1);

        loadIdentity();

        addTransformation(glm::translate(glm::mat4(1.0f), pos));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.09)));

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
        pos += dir*velocity;
        light->pos = pos;
        light->fallOff = glm::vec3(0, glm::distance(pos, initialPos)/10, 0);

        if (glm::distance(pos, initialPos) >= power){
            die();
            return;
        }

        CollisionManager::checkCollisionWithGround(this);
        if (!finished)
            CollisionManager::checkCollisionWithEnemy(this);
    }

    void onCollision(Object3D * obj){
        die();
        return;
    }

private:
    glm::vec3 initialPos, pos;
    glm::vec3 dir;
    float velocity, power;
    Light * light;

    bool finished = false;
    std::list<Object3D *>::iterator collisionListPos;

    void die(){
        CollisionManager::removeShot(collisionListPos);
        LightManager::removeLight(light);
        parent->removeChild(this);
        finished = true;
    }
};

#endif