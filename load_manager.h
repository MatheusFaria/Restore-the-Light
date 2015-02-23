#ifndef LOAD_MANAGER_H
#define LOAD_MANAGER_H

#include <string>
#include <map>

#include "shader.h"
#include "mesh.h"

class LoadManager {
public:
    static Shader * getShader(std::string vertex, std::string fragment);
    static Mesh * getMesh(std::string name);

private:
    static std::map<std::string, Shader *> shaders;
    static std::map<std::string, Mesh *> meshes;
};

#endif