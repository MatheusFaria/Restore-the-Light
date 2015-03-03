#include "load_manager.h"

std::map<std::string, Shader *> LoadManager::shaders;
std::map<std::string, Mesh *> LoadManager::meshes;
std::map<std::string, Image *> LoadManager::images;

Shader * LoadManager::getShader(std::string vertex, std::string fragment){
    std::string name = vertex + fragment;
    if (shaders.find(name) == shaders.end()){
        shaders[name] = new Shader(vertex, fragment);
    }
    return shaders[name];
}

Mesh * LoadManager::getMesh(std::string name){
    if (meshes.find(name) == meshes.end()){
        meshes[name] = new Mesh(name);
        meshes[name]->init();
    }
    return meshes[name];
}

Image * LoadManager::getImage(std::string name){
    if (images.find(name) == images.end()){
        images[name] = new Image(name);
        images[name]->loadBMP();
    }
    return images[name];
}