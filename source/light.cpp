#include "light.h"

#include "log.h"

#include "glm/gtc/type_ptr.hpp"

Light::Light() : Light(glm::vec3(0), glm::vec3(1), glm::vec3(1, 0, 0), UNDEFINED){}

Light::Light(glm::vec3 _pos, glm::vec3 _color, glm::vec3 _fallOff, int _type):
pos(_pos), color(_color), fallOff(_fallOff), type(_type){
}

void Light::load(const GLint posHandle, const GLint colorHandle, const GLint fallOffHandle){
    glUniform4f(posHandle, pos.x, pos.y, pos.z, type);
    glUniform3f(colorHandle, color.r, color.g, color.b);
    glUniform3f(fallOffHandle, fallOff.x, fallOff.y, fallOff.z);
}


std::list<Light *> LightManager::lights;

void LightManager::addLight(Light * light){
    lights.push_back(light);
}

std::list<Light *> LightManager::getLights(){
    return lights;
}