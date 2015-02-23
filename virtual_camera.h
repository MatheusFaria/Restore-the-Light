#ifndef __VIRTUAL_CAMERA_H__
#define __VIRTUAL_CAMERA_H__

#include <vector>
#include "glm/glm.hpp"

class Cam{
public:
    Cam();
    Cam(glm::vec3 _eye, glm::vec3 _lookAt, glm::vec3 _upVector);

    glm::mat4 getViewMatrix();

    glm::mat4 projectionMatrix;
    glm::vec3 eye, lookAt, upVector;
};

class CamManager{
public:
    static Cam * getCam(int id);
    static int addCam(Cam * cam);

    static Cam * currentCam();
    static void setCam(int id);

private:
    static int currentCamId;
    static std::vector<Cam *> cameras;
};

#endif