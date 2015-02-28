#include "virtual_camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "log.h"

# define M_PI 3.141592

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

float degToRad(float angle){
    return angle*M_PI / 180;
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
    lookAt.x = cos(degToRad(phi))*cos(degToRad(theta)) + eye.x;
    lookAt.y = sin(degToRad(phi)) + eye.y;
    lookAt.z = cos(degToRad(phi))*cos(degToRad(90 - theta)) + eye.z;
}

void Cam::turnOffY(){
    updateY = false;
}

std::vector<Cam *> CamManager::cameras;
int CamManager::currentCamId = -1;
Cam * CamManager::getCam(int id){
    if (id >= 0 && id < cameras.size()){
        return cameras[id];
    }
    Log::error("CamManager::getCam", "Index out of range", "", "");
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