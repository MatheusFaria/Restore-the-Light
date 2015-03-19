#include "light.h"

#include <algorithm>
#include <iostream>

Light::Light() 
    : color(glm::vec3(1)), pos(glm::vec3(0)), dir(glm::vec3(0, 0, 0)), fallOff(glm::vec3(1, 0, 0)), cutOffAngle(360.0f), type(UNDEFINED){}

// Directional Light Constructor
Light::Light(glm::vec3 _color, glm::vec3 _dir) 
    : color(_color), pos(glm::vec3(0)), dir(_dir), fallOff(glm::vec3(1, 0, 0)), cutOffAngle(360.0f), type(Light::DIRECTIONAL){}

// Point Light Constructor
Light::Light(glm::vec3 _color, glm::vec3 _pos, glm::vec3 _fallOff) 
    : color(_color), pos(_pos), dir(glm::vec3(0, 0, 0)), fallOff(_fallOff), cutOffAngle(360.0f), type(POINT_LIGHT){}

// Spot Light Constructor
Light::Light(glm::vec3 _color, glm::vec3 _pos, glm::vec3 _dir, glm::vec3 _fallOff, float _cutOffAngle)
    : color(_color), pos(_pos), dir(_dir), fallOff(_fallOff), cutOffAngle(_cutOffAngle), type(SPOT_LIGHT){}

Light::Light(glm::vec3 _color, glm::vec3 _pos, glm::vec3 _dir, 
    glm::vec3 _fallOff, float _cutOffAngle, int _type) 
    : color(_color), pos(_pos), dir(_dir), fallOff(_fallOff), cutOffAngle(_cutOffAngle), type(_type){}

void Light::load(const GLint posHandle, const GLint dirHandle, const GLint colorHandle,
    const GLint fallOffHandle, const GLint cutOffAngleHandle){
    glUniform4f(posHandle, pos.x, pos.y, pos.z, type);
    glUniform3f(dirHandle, dir.x, dir.y, dir.z);
    glUniform3f(colorHandle, color.x, color.y, color.z);
    glUniform3f(fallOffHandle, fallOff.x, fallOff.y, fallOff.z);
    glUniform1f(cutOffAngleHandle, cutOffAngle*3.141592f/ 180.0f);
}

float Light::lightAreaRadius(){
    float C = std::max(color.x, std::max(color.y, color.z));
    float I = 0.0001f;
    if (fallOff.z == 0.0f){
        if (fallOff.y == 0.0f){
            if (fallOff.x == 0.0f){
                return 0;
            }
            return 1 / fallOff.x;
        }
        return (-fallOff.x + 256 * I * C) / fallOff.y;
    }
    return (-fallOff.y + sqrt(fallOff.y*fallOff.y - 4 * fallOff.z*(fallOff.x - 256 * I * C)))/(2.0*fallOff.z);
}

std::list<Light *> LightManager::pointLights;
std::list<Light *> LightManager::spotLights;
std::list<Light *> LightManager::directionalLights;

void LightManager::addLight(Light * light){
    switch (light->type){
    case Light::POINT_LIGHT:
        pointLights.push_back(light);
        break;
    case Light::SPOT_LIGHT:
        spotLights.push_back(light);
        break;
    case Light::DIRECTIONAL:
        directionalLights.push_back(light);
        break;
    }
}

void LightManager::removeLight(Light * light){
    switch (light->type){
    case Light::POINT_LIGHT:
        for (std::list<Light *>::iterator i = pointLights.begin(); i != pointLights.end(); i++){
            if ((*i) == light){
                pointLights.erase(i);
                break;
            }
        }
        break;
    case Light::SPOT_LIGHT:
        for (std::list<Light *>::iterator i = spotLights.begin(); i != spotLights.end(); i++){
            if ((*i) == light){
                spotLights.erase(i);
                break;
            }
        }
        break;
    case Light::DIRECTIONAL:
        for (std::list<Light *>::iterator i = directionalLights.begin(); i != directionalLights.end(); i++){
            if ((*i) == light){
                directionalLights.erase(i);
                break;
            }
        }
        break;
    }
}

std::list<Light *> LightManager::getPointLights(){
    return pointLights;
}

std::list<Light *> LightManager::getSpotLights(){
    return spotLights;
}

std::list<Light *> LightManager::getDirectionalLights(){
    return directionalLights;
}
