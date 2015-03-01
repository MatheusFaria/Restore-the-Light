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

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "scene.h"


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

        pos = gameMap->getCubePos(currentCube);
        pos.y += 2;

        setControlMode(FPS);
        velocity = 0.3f;
    }

    void drawObject(){
        glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);
        glUniform3f(shader->getHandle("uLightPos"), g_light.x, g_light.y, g_light.z);
        glUniform3f(shader->getHandle("uEye"), CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);
        //glUniform3f(shader->getHandle("uEye"),
        //    CamManager::currentCam()->getViewVector().x,
        //    CamManager::currentCam()->getViewVector().y,
        //    CamManager::currentCam()->getViewVector().z);

        Material::SetMaterial(Material::GOLD, shader);
        loadIdentity();

        addTransformation(glm::translate(glm::mat4(1.0f), pos));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.5)));
        bindModelMatrix("uModelMatrix");

        if (!isFPS())
            drawElements();
    }

    void checkInput(GLFWwindow * window){
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
            setControlMode(FPS);
        }
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
            setControlMode(ISO);
        }
        
        if (isFPS()){
            FPSControl(window);
        }
        else if (isISO()){
            ISOControl(window);
        }
    }

    bool isFPS(){
        return controlMode == FPS;
    }

    bool isISO(){
        return controlMode == ISO;
    }

private:
    glm::vec3 pos;
    float velocity;
    int controlMode;
    const int FPS = 0, ISO = 1, TOP = 2;
    int currentCube;
    GameMap * gameMap;

    static const int FOWARD_DIR = -1;
    static const int BACKWARD_DIR = 1;
    static const int LEFT_DIR = 1;
    static const int RIGHT_DIR = -1;

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
        pos += CamManager::currentCam()->getViewVector()*(strafeDir*velocity);

        if (gameMap->thereIsCube(pos)){
            CamManager::currentCam()->zoom(zoomDir, velocity);
            CamManager::currentCam()->strafe(strafeDir, velocity);

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
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
            dir += glm::vec3(0, 0, 1);
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
            dir += glm::vec3(-1, 0, 0);
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
            dir += glm::vec3(1, 0, 0);
        }
        pos += dir*velocity;

        if (!gameMap->thereIsCube(pos)){
            pos = oldPos;
        }

        CamManager::currentCam()->lookAt = pos;
        CamManager::currentCam()->eye = glm::vec3(pos.x + 5, pos.y + 2, pos.z + 4);
    }

    void setControlMode(int val){
        if (val == FPS){
            controlMode = FPS;
            CamManager::setCam(FPS_CAM);
            //CamManager::currentCam()->turnOffY();
            CamManager::currentCam()->setAngles(-180, 0);
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
};

#endif