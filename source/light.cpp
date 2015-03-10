#include "light.h"

# define M_PI 3.141592

#include "log.h"

#include "glm/gtc/type_ptr.hpp"

Light::Light() 
    : Light(glm::vec3(1), glm::vec3(0), glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), 360.0f, UNDEFINED){}

// Directional Light Constructor
Light::Light(glm::vec3 _color, glm::vec3 _dir) 
    : Light(color, glm::vec3(0), _dir, glm::vec3(1, 0, 0), 360.0f, DIRECTIONAL){}

// Point Light Constructor
Light::Light(glm::vec3 _color, glm::vec3 _pos, glm::vec3 _fallOff) 
    : Light(_color, _pos, glm::vec3(0, 0, 0), _fallOff, 360.0f, POINT_LIGHT){}

// Spot Light Constructor
Light::Light(glm::vec3 _color, glm::vec3 _pos, glm::vec3 _dir, glm::vec3 _fallOff, float _cutOffAngle)
    : Light(_color, _pos, _dir, _fallOff, _cutOffAngle, SPOT_LIGHT){}

Light::Light(glm::vec3 _color, glm::vec3 _pos, glm::vec3 _dir, 
    glm::vec3 _fallOff, float _cutOffAngle, int _type) :
    color(_color), pos(_pos), dir(_dir), fallOff(_fallOff), cutOffAngle(_cutOffAngle), type(_type){
}

void Light::load(const GLint posHandle, const GLint dirHandle, const GLint colorHandle,
    const GLint fallOffHandle, const GLint cutOffAngleHandle){
    glUniform4f(posHandle, pos.x, pos.y, pos.z, type);
    glUniform3f(dirHandle, dir.x, dir.y, dir.z);
    glUniform3f(colorHandle, color.r, color.g, color.b);
    glUniform3f(fallOffHandle, fallOff.x, fallOff.y, fallOff.z);
    glUniform1f(cutOffAngleHandle, cutOffAngle*M_PI/180.0f);
}


std::list<Light *> LightManager::lights;

void LightManager::addLight(Light * light){
    lights.push_back(light);
}

std::list<Light *> LightManager::getLights(){
    return lights;
}