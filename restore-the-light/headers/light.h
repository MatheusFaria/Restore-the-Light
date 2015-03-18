#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <list>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_PURE
#include "glm/glm.hpp"

#include "GLIncludes.h"

class Light{
public:
    Light();
    Light(glm::vec3 _color, glm::vec3 _pos, glm::vec3 _dir, glm::vec3 _fallOff, float _cutOffAngle, int _type);

    Light(glm::vec3 _color, glm::vec3 _dir);
    Light(glm::vec3 _color, glm::vec3 _pos, glm::vec3 _fallOff);
    Light(glm::vec3 _color, glm::vec3 _pos, glm::vec3 _dir, glm::vec3 _fallOff, float _cutOffAngle);


    void load(const GLint posHandle, const GLint dirHandle, const GLint colorHandle,
        const GLint fallOffHandle, const GLint cutOffAngleHandle);

    float lightAreaRadius();


    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 direction;
    glm::vec3 color;
    glm::vec3 fallOff;
    float cutOffAngle;
    int type;

    static const int DIRECTIONAL = 0;
    static const int POINT_LIGHT = 1;
    static const int SPOT_LIGHT = 2;
    static const int UNDEFINED = -1;
};

class LightManager{
public:
    static void addLight(Light * light);
    static void removeLight(Light * light);

    static std::list<Light *> getPointLights();
    static std::list<Light *> getSpotLights();
    static std::list<Light *> getDirectionalLights();

private:
    static std::list<Light *> LightManager::pointLights;
    static std::list<Light *> LightManager::spotLights;
    static std::list<Light *> LightManager::directionalLights;
};

#endif