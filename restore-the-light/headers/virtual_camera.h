#ifndef __VIRTUAL_CAMERA_H__
#define __VIRTUAL_CAMERA_H__

#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_PURE
#include "glm/glm.hpp"

#define FPS_CAM 0
#define ISO_CAM 1
#define TOP_CAM 2

class Cam{
public:
    Cam();
    Cam(glm::vec3 _eye, glm::vec3 _lookAt, glm::vec3 _upVector);

    glm::mat4 getViewMatrix();
    glm::vec3 getViewVector();
    glm::vec3 getStrafeVector();

    void strafe(int direction, float velocity);
    void zoom(int direction, float velocity);
    
    void setAngles(float _theta, float _phi);
    float getTheta();
    float getPhi();
    
    void turnOffY();
    void turnOnY();

    glm::mat4 projectionMatrix;
    glm::vec3 eye, lookAt, upVector;

private:
    float theta, phi;
    void updateLookAt();
    bool updateY;
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