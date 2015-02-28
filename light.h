#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <vector>

#include "GLSL.h"
#include "glm/glm.hpp"

class Light{
public:
    Light();
    Light(glm::mat4 matrix);
    Light(glm::vec3 _pos, glm::vec3 _color, glm::vec3 _fallOff, int _type);

    /*   pos.x     pos.y    pos.z      TYPE
      fallOff.x fallOff.y fallOff.z     0
       color.x   color.y   color.z      0
          0         0        0          1*/
    glm::mat4 getLightMatrix();

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
    static void init();
    static void addLight(Light * light, int pos);

    static Light * getLight(int pos);

    static void loadLights(const GLint handle);

private:
    static const int MAX_SIZE = 20;
    static std::vector<glm::mat4> lights;

};

#endif