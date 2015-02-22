#include "object3d.h"

/*
* Author: Matheus de Sousa Faria
* CPE 471 - Introduction to Computer Graphics
* Program 3
*/

#include <iostream>

Object3D::Object3D() {}

Object3D::Object3D(Mesh * _mesh, Shader * _shader): mesh(_mesh), shader(_shader) {
    parent = NULL;
}

void Object3D::addTransformation(glm::mat4 matrix) {
    modelTransform = matrix*modelTransform;
}

void Object3D::loadArrayBuffer(std::string bufferName, std::vector<float> v){
    if (arrayBuffers.find(bufferName) == arrayBuffers.end()){
        arrayBuffers[bufferName] = 0;
        glGenBuffers(1, &arrayBuffers[bufferName]);
        glBindBuffer(GL_ARRAY_BUFFER, arrayBuffers[bufferName]);
        glBufferData(GL_ARRAY_BUFFER, v.size()*sizeof(float), &v[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (arrayBuffers[bufferName] == 0)
        std::cout << "Could not load array buffer: " << bufferName << "\n";
}

GLuint Object3D::getArrayBuffer(std::string bufferName){
    if (arrayBuffers.find(bufferName) == arrayBuffers.end()){
        std::cout << "Array buffer not loaded: " << bufferName << "\n";
        return 0;
    }

    return arrayBuffers[bufferName];
}

void Object3D::loadElementBuffer(std::vector<unsigned int> v){
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, v.size()*sizeof(unsigned int), &v[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Object3D::loadElementBuffer(){
    loadElementBuffer(mesh->getIndices());
}

GLuint Object3D::getElementBuffer(){
    return elementBuffer;
}

void Object3D::loadVertexBuffer(std::string bufferName){
    loadArrayBuffer(bufferName, mesh->getVertices());
}

void Object3D::loadNormalBuffer(std::string bufferName){
    loadArrayBuffer(bufferName, mesh->getNormals());
}

Mesh * Object3D::getMesh(){
    return mesh;
}

void Object3D::setParent(Object3D * _parent){
    parent = _parent;
}

glm::mat4 Object3D::getModelMatrix(){
    return modelTransform;
}

glm::mat4 Object3D::getViewMatrix(){
    return viewTransform;
}

//void Object3D::draw(){}


void Object3D::setViewMatrix(glm::mat4 matrix){
    viewTransform = matrix;
}