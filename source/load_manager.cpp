#include "load_manager.h"

std::map<std::string, Shader *> LoadManager::shaders;
std::map<std::string, Mesh *> LoadManager::meshes;
std::map<std::string, Image *> LoadManager::images;


const std::string LoadManager::shaderFolder = "shaders";
const std::string LoadManager::meshFolder = "objects";
const std::string LoadManager::imageFolder = "textures";

std::string joinPath(std::string folder, std::string file){
    folder.append("/");
    folder.append(file);
    return folder;
}

Shader * LoadManager::getShader(std::string vertex, std::string fragment){
    std::string name = vertex + fragment;
    if (shaders.find(name) == shaders.end()){
        loadShader(vertex, fragment);
    }
    return shaders[name];
}

bool LoadManager::loadShader(std::string vertex, std::string fragment){
    std::string name = vertex + fragment;
    if (shaders.find(name) == shaders.end()){
        Shader * s = new Shader(joinPath(shaderFolder, vertex), joinPath(shaderFolder, fragment));
        bool ret = s->load();
        if (ret)
            shaders[name] = s;
        else
            return false;
    }
    return true;
}

Mesh * LoadManager::getMesh(std::string name, bool resize){
    if (meshes.find(name) == meshes.end()){
        meshes[name] = new Mesh(joinPath(meshFolder, name));
        meshes[name]->init(resize);
    }
    return meshes[name];
}

Image * LoadManager::getImage(std::string name){
    if (images.find(name) == images.end()){
        images[name] = new Image(joinPath(imageFolder, name));
        images[name]->loadBMP();
    }
    return images[name];
}