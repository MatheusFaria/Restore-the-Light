#include "virtual_camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "log.h"

Cam::Cam() : Cam(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)){
}

Cam::Cam(glm::vec3 _eye, glm::vec3 _lookAt, glm::vec3 _upVector):
eye(_eye), lookAt(_lookAt), upVector(_upVector){
    projectionMatrix = glm::mat4(1.0f);
}

glm::mat4 Cam::getViewMatrix(){
    return glm::lookAt(eye, lookAt, upVector);
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