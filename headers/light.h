#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <list>

#include "GLSL.h"
#include "glm/glm.hpp"

class Light{
public:
    Light();
    Light(glm::vec3 _pos, glm::vec3 _color, glm::vec3 _fallOff, int _type);

    void load(const GLint posHandle, const GLint colorHandle, const GLint fallOffHandle);

    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 fallOff;
    int type;

    static const int DIRECTIONAL = 0;
    static const int POINT_LIGHT = 1;
    static const int SPOT_LIGHT = 2;
    static const int UNDEFINED = -1;
};

class LightManager{
public:
    static void addLight(Light * light);

    static std::list<Light *> getLights();

private:
    static std::list<Light *> LightManager::lights;
};

#endif