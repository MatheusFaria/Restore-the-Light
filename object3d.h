#ifndef OBJECT3D_H
#define OBJECT3D_H

/*
* Author: Matheus de Sousa Faria
* CPE 471 - Introduction to Computer Graphics
* Program 3
*/

#include <map>
#include <vector>

#include "mesh.h"
#include "shader.h"
#include "glm/glm.hpp"

class Object3D{
public:
    Object3D();
    Object3D(Mesh * _mesh, Shader * _shader);

    void addTransformation(glm::mat4 matrix);
    void loadArrayBuffer(std::string bufferName, std::vector<float> v);
    void loadElementBuffer(std::vector<unsigned int> v);
    void loadElementBuffer();
    void loadVertexBuffer(std::string bufferName);
    void loadNormalBuffer(std::string bufferName);
    void setParent(Object3D * _parent);
    void setViewMatrix(glm::mat4 matrix);
    Mesh * getMesh();
    GLuint getElementBuffer();
    GLuint getArrayBuffer(std::string bufferName);
    glm::mat4 getModelMatrix();
    glm::mat4 getViewMatrix();

    //virtual void draw();

protected:
    Mesh * mesh;
    Shader * shader;

private:
    Object3D * parent;
    std::vector<Object3D *> children;

    std::map<std::string, GLuint> arrayBuffers;
    GLuint elementBuffer;

    glm::mat4 modelTransform = glm::mat4(1.0f);
    glm::mat4 viewTransform = glm::mat4(1.0f);
};

#endif