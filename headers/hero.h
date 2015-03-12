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

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "scene.h"


class Hero : public Object3D{
public:
    Hero(GameMap * _map, int cubeIndex) : gameMap(_map), currentCube(cubeIndex){
    }

    void init(){
        mesh = LoadManager::getMesh("sphere-tex.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadTextureBuffer("texBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader("vert-map.glsl", "frag-map.glsl");

        pos = gameMap->getCubePos(currentCube);
        pos.y += 2;

        setControlMode(FPS);
        velocity = 0.3f;

        light = new Light(glm::vec3(1), pos, glm::vec3(0, 0.7, 0));
        LightManager::addLight(light);

        gLight = new Light(glm::vec3(1, 1, 1), glm::vec3(0, -1, 0));
    }

    void drawObject(){
        light->pos = pos + glm::vec3(0.00001);

        if (!isFPS()){
            Material::SetMaterial(Material::GOLD, shader);
            glUniform3f(shader->getHandle("UeColor"), 1, 1, 1);

            enableAttrArray2f("aTexCoord", "texBufObj");

            glUniform1i(shader->getHandle("uCompleteGlow"), 1);
            glUniform1i(shader->getHandle("uApplyTexture"), 0);

            loadIdentity();

            addTransformation(glm::translate(glm::mat4(1.0f), pos));
            addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.5)));
            bindModelMatrix("uModelMatrix");
        
        
            drawElements();
        }
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
            LightManager::addLight(gLight);
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
            LightManager::addLight(new Light(glm::vec3(rand_float(), rand_float(), rand_float()),
                pos, glm::vec3(0, 0.3, 0)));
        }
        
        if (isFPS()){
            FPSControl(window);
        }
        else if (isISO()){
            ISOControl(window);
        }
    }

    bool isFPS(){
        return (controlMode == FPS || controlMode == FREE);
    }

    bool isISO(){
        return controlMode == ISO;
    }

private:
    glm::vec3 pos;
    float velocity;
    int controlMode;
    const int FPS = 0, 
              ISO = 1, 
              TOP = 2,
              FREE = -1;
    int currentCube;
    GameMap * gameMap;
    Light * light, *gLight;

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

        if (gameMap->thereIsCube(pos) || controlMode == FREE){
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
            CamManager::currentCam()->turnOffY();
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
        else if (val == FREE){
            controlMode = FREE;
            CamManager::setCam(FPS_CAM);
            CamManager::currentCam()->turnOnY();
            CamManager::currentCam()->setAngles(-90, 0);
            CamManager::currentCam()->eye = pos;
            CamManager::currentCam()->lookAt = glm::vec3(-1, pos.y, pos.z);
        }
        else{
            Log::error("Hero::setControlMode", "No such control mode option", "", "");
        }
    }
};

#endif