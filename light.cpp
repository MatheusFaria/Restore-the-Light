#include "light.h"

#include "log.h"

#include "glm/gtc/type_ptr.hpp"

Light::Light() : Light(glm::vec3(0), glm::vec3(1), glm::vec3(1, 0, 0), UNDEFINED){}

Light::Light(glm::mat4 matrix){
    pos = glm::vec3(matrix[0]);
    fallOff = glm::vec3(matrix[1]);
    color = glm::vec3(matrix[2]);
    type = matrix[0][3];
}

Light::Light(glm::vec3 _pos, glm::vec3 _color, glm::vec3 _fallOff, int _type):
pos(_pos), color(_color), fallOff(_fallOff), type(_type){
}

/*   
  pos.x     pos.y    pos.z      TYPE
fallOff.x fallOff.y fallOff.z     0
 color.x   color.y   color.z      0
  0         0          0          1
*/
glm::mat4 Light::getLightMatrix(){
    glm::mat4 matrix = glm::mat4(1.0f);

    matrix[0] = glm::vec4(pos, type);
    matrix[1] = glm::vec4(fallOff, 0);
    matrix[2] = glm::vec4(color, 0);

    return matrix;
}



const int LightManager::MAX_SIZE;
std::vector<glm::mat4> LightManager::lights;

void LightManager::init(){
    Light * l = new Light();
    for (int i = 0; i < MAX_SIZE; i++){
        addLight(l, i);
    }
}

void LightManager::addLight(Light * light, int pos){
    if (pos >= 0 && pos < MAX_SIZE){
        lights[pos] = light->getLightMatrix();
        return;
    }

    Log::error("LightManager::addLight", "Unvalid pos", "", "");
}

Light * LightManager::getLight(int pos){
    if (pos >= 0 && pos < MAX_SIZE){
        return &Light(lights[pos]);
    }
    return NULL;
}

void LightManager::loadLights(const GLint handle){
    glUniformMatrix4fv(handle, MAX_SIZE, GL_FALSE, glm::value_ptr(lights[0]));
}