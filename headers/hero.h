#ifndef __HERO_H__
#define __HERO_H__

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

class Hero : public Object3D{
public:
    Hero(GameMap * _map, int cubeIndex) : gameMap(_map), currentCube(cubeIndex){
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

        setControlMode(FPS);
        velocity = 0.3f;
        floatVeloctiy = 0.008f;
        shotTime = 0;
        lastDir = glm::vec3(0, 0, -1);

        light = new Light(glm::vec3(1), pos, glm::vec3(0, -1, 0), glm::vec3(0, 0.3, 0), 45.0f);
        LightManager::addLight(light);

        gLight = new Light(glm::vec3(1, 1, 1), glm::vec3(0, -1, 0));

        setCollisionsMask(2, 0);
        CollisionManager::setHero(this);

        gameMap->litCube(currentCube);
    }

    void drawObject(){
        light->pos = glm::vec3(pos.x, pos.y + 8, pos.z);

        loadIdentity();

        addTransformation(glm::translate(glm::mat4(1.0f), pos));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.5)));

        if (!isFPS()){
            glUseProgram(shader->getId());

            enableAttrArray3f("aPosition", "posBufObj");
            enableAttrArray3f("aNormal", "norBufObj");
            bindElements();

            Material::SetMaterial(Material::GOLD, shader);
            glUniform3f(shader->getHandle("UeColor"), 1, 1, 1);

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

        CollisionManager::checkCollisionWithEnemy(this);
    }

    float rand_float(){
        return ((float)rand()) / ((float)RAND_MAX);
    }

    void checkInput(GLFWwindow * window){
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
            setControlMode(FPS);
        }
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
            setControlMode(ISO);
        }
        else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS){
            setControlMode(FREE);
        }
        
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS){
            gameMap->lightAllTheWay();
        }
        else if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS){
            gameMap->clearPath();
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
            LightManager::addLight(new Light(glm::vec3(rand_float(), rand_float(), rand_float()),
                pos, glm::vec3(0, 0.6, 0)));
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
            if (glfwGetTime() - shotTime >= shotCoolDown){
                shotTime = glfwGetTime();
                lightShot();
            }
        }
        
        if (isFPS()){
            FPSControl(window);
        }
        else if (isISO()){
            updateFloatMovement();
            ISOControl(window);
        }
    }

    bool isFPS(){
        return (controlMode == FPS || controlMode == FREE);
    }

    bool isISO(){
        return controlMode == ISO;
    }

    void onCollision(Object3D * obj){
        if (obj->getCollisionTypeMask() == 4){
        }
    }

private:
    static const int FOWARD_DIR = -1;
    static const int BACKWARD_DIR = 1;
    static const int UP_DIR = 1;
    static const int DOWN_DIR = -1;
    static const int LEFT_DIR = 1;
    static const int RIGHT_DIR = -1;

    glm::vec3 pos, initialPos, lastDir;
    int currentCube;

    float velocity;
    float floatVeloctiy;

    int floatDirection = UP_DIR;
    float defaultHeight = 2, heightOffset = 0.4;

    double shotCoolDown = 0.5, shotTime;

    int controlMode;
    const int FPS = 0,
        ISO = 1,
        TOP = 2,
        FREE = -1;

    GameMap * gameMap;
    Light * light, *gLight;

    void FPSControl(GLFWwindow * window){
        glm::vec3 oldPos = pos;
        int zoomDir = 0, strafeDir = 0;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
            zoomDir = FOWARD_DIR;
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
            zoomDir = BACKWARD_DIR;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
            strafeDir = LEFT_DIR;
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
            strafeDir = RIGHT_DIR;
        }

        pos += CamManager::currentCam()->getViewVector()*(zoomDir*velocity);
        pos += CamManager::currentCam()->getStrafeVector()*(strafeDir*velocity);
        pos.y = oldPos.y;

        if (gameMap->thereIsCube(pos) || controlMode == FREE){
            CamManager::currentCam()->zoom(zoomDir, velocity);
            CamManager::currentCam()->strafe(strafeDir, velocity);

            if (controlMode == FPS){
                currentCube = gameMap->getCubePos(pos);
                gameMap->litCube(currentCube);
            }

            pos = CamManager::currentCam()->eye;
        }
        else{
            pos = oldPos;
        }
    }

    void ISOControl(GLFWwindow * window){
        glm::vec3 dir = glm::vec3(0), oldPos = pos;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
            dir += glm::vec3(0, 0, -1);
            lastDir = dir;
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
            dir += glm::vec3(0, 0, 1);
            lastDir = dir;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
            dir += glm::vec3(-1, 0, 0);
            lastDir = dir;
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
            dir += glm::vec3(1, 0, 0);
            lastDir = dir;
        }
        pos += dir*velocity;

        if (!gameMap->thereIsCube(pos)){
            pos = oldPos;
        }
        else {
            currentCube = gameMap->getCubePos(pos);
            gameMap->litCube(currentCube);
        }

        CamManager::currentCam()->lookAt = glm::vec3(pos.x, initialPos.y, pos.z);
        CamManager::currentCam()->eye = glm::vec3(pos.x + 5, initialPos.y + 2, pos.z + 4);
    }

    void updateFloatMovement(){
        pos.y += floatDirection*floatVeloctiy;

        if (pos.y >= initialPos.y + heightOffset){
            floatDirection = DOWN_DIR;
        }
        else if (pos.y <= initialPos.y - heightOffset){
            floatDirection = UP_DIR;
        }
    }

    void setControlMode(int val){
        if (val == FPS || val == FREE){
            pos = glm::vec3(pos.x, initialPos.y, pos.z);
            CamManager::setCam(FPS_CAM);
            if (val == FPS){
                controlMode = FPS;
                CamManager::currentCam()->turnOffY();
            }
            else{
                controlMode = FREE;
                CamManager::currentCam()->turnOnY();
            }
            CamManager::currentCam()->setAngles(-90, 0);
            CamManager::currentCam()->eye = pos;
            CamManager::currentCam()->lookAt = glm::vec3(-1, pos.y, pos.z);
        }
        else if (val == ISO){
            controlMode = ISO;
            CamManager::setCam(ISO_CAM);
            CamManager::currentCam()->lookAt = pos;
            CamManager::currentCam()->setAngles(225, -15);
            CamManager::currentCam()->eye = glm::vec3(pos.x + 5, pos.y + 2, pos.z + 4);
        }
        else if (val == TOP){
            controlMode = TOP;
            CamManager::setCam(TOP_CAM);
        }
        else{
            Log::error("Hero::setControlMode", "No such control mode option", "", "");
        }
    }

    void lightShot(){
        glm::vec3 dir;
        if (isFPS()){
            dir = CamManager::currentCam()->getViewVector();
            dir = -dir;
        }
        else{
            dir = lastDir;
        }
        LightShot * l = new LightShot(pos, dir, 0.7, 10);
        l->init();

        gameMap->addChild(l);
    }
};

#endif