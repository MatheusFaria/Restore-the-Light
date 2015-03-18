#include "virtual_camera.h"

#include <iostream>

#define degreesToRadians(x) (x*(3.141592f/180.0f))
#define GLM_FORCE_RADIANS
#define GLM_FORCE_PURE
#include "glm/gtc/matrix_transform.hpp"

Cam::Cam() : Cam(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)){
}

Cam::Cam(glm::vec3 _eye, glm::vec3 _lookAt, glm::vec3 _upVector):
eye(_eye), lookAt(_lookAt), upVector(_upVector){
    projectionMatrix = glm::mat4(1.0f);
    updateY = true;
}

glm::mat4 Cam::getViewMatrix(){
    return glm::lookAt(eye, lookAt, upVector);
}

glm::vec3 Cam::getViewVector(){
    return eye - lookAt;
}

glm::vec3 Cam::getStrafeVector(){
    return glm::cross(getViewVector(), upVector);
}

void Cam::strafe(int direction, float velocity){
    updateLookAt();
    glm::vec3 strafe = glm::cross(getViewVector(), upVector);
    float dir = (float)direction;
    eye += strafe*velocity*dir;
    lookAt += strafe*velocity*dir;
}

void Cam::zoom(int direction, float velocity){
    updateLookAt();
    float dir = (float)direction;
    eye.x += getViewVector().x*velocity*dir;
    if (updateY)
        eye.y += getViewVector().y*velocity*dir;
    eye.z += getViewVector().z*velocity*dir;
    lookAt += getViewVector()*velocity*dir;
}

void Cam::setAngles(float _theta, float _phi){
    theta = _theta;
    phi = _phi;
    updateLookAt();
}

float Cam::getTheta(){
    return theta;
}

float Cam::getPhi(){
    return phi;
}

void Cam::updateLookAt(){
    lookAt.x = cos(degreesToRadians(phi))*cos(degreesToRadians(theta)) + eye.x;
    lookAt.y = sin(degreesToRadians(phi)) + eye.y;
    lookAt.z = cos(degreesToRadians(phi))*cos(degreesToRadians(90 - theta)) + eye.z;
}

void Cam::turnOffY(){
    updateY = false;
}

void Cam::turnOnY(){
    updateY = true;
}

std::vector<Cam *> CamManager::cameras;
int CamManager::currentCamId = -1;
Cam * CamManager::getCam(int id){
    if (id >= 0 && id < cameras.size()){
        return cameras[id];
    }
    std::cout << "CamManager::getCam: Index out of range\n";
}

int CamManager::addCam(Cam * cam){
    cameras.push_back(cam);
    return cameras.size() - 1;
}

Cam * CamManager::currentCam(){
    return getCam(currentCamId);
}

void CamManager::setCam(int id){
    currentCamId = id;
}