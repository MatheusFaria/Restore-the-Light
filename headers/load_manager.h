#ifndef LOAD_MANAGER_H
#define LOAD_MANAGER_H

#include <string>
#include <map>

#include "shader.h"
#include "mesh.h"
#include "image.h"

class LoadManager {
public:
    static Shader * getShader(std::string vertex, std::string fragment);
    static bool loadShader(std::string vertex, std::string fragment);
    static Mesh * getMesh(std::string name);
    static Image * getImage(std::string name);

private:
    static std::map<std::string, Shader *> shaders;
    static std::map<std::string, Mesh *> meshes;
    static std::map<std::string, Image *> images;

    static const std::string shaderFolder;
    static const std::string meshFolder;
    static const std::string imageFolder;
};

#endif