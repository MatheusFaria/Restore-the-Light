#include "object3d.h"

#include <iostream>

#include "GLSL.h"
#include "glm/gtc/type_ptr.hpp"
#include "virtual_camera.h"

Object3D::Object3D() : Object3D(NULL, NULL){
}

Object3D::Object3D(Mesh * _mesh, Shader * _shader) : mesh(_mesh), shader(_shader) {
    parent = NULL;
    matrixStack.push(glm::mat4(1.0f));
    modelTransform = glm::mat4(1.0f);
}

void Object3D::addTransformation(glm::mat4 matrix) {
    modelTransform = modelTransform*matrix;
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

void Object3D::loadTextureBuffer(std::string bufferName){
    loadArrayBuffer(bufferName, mesh->getTexCoords());
}

Mesh * Object3D::getMesh(){
    return mesh;
}

void Object3D::setParent(Object3D * _parent){
    parent = _parent;
}

glm::mat4 Object3D::getModelMatrix(){
    if (parent != NULL)
        return parent->getModelMatrix()*modelTransform;
    else
        return modelTransform;
}

void Object3D::enableAttrArray3f(std::string aHandle, std::string bufHandle){
    GLSL::enableVertexAttribArray(shader->getHandle(aHandle));
    glBindBuffer(GL_ARRAY_BUFFER, getArrayBuffer(bufHandle));
    glVertexAttribPointer(shader->getHandle(aHandle), 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Object3D::enableAttrArray2f(std::string aHandle, std::string bufHandle){
    GLSL::enableVertexAttribArray(shader->getHandle(aHandle));
    glBindBuffer(GL_ARRAY_BUFFER, getArrayBuffer(bufHandle));
    glVertexAttribPointer(shader->getHandle(aHandle), 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void Object3D::disableAttrArray(std::string aHandle){
    GLSL::disableVertexAttribArray(shader->getHandle(aHandle));
}

void Object3D::loadIdentity(){
    modelTransform = glm::mat4(1.0f);
}

void Object3D::unbindAll(){
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void Object3D::bindElements(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getElementBuffer());
}

void Object3D::drawElements(){
    glDrawElements(GL_TRIANGLES, (int)mesh->getIndices().size(), GL_UNSIGNED_INT, 0);
}


void Object3D::addChild(Object3D * child){
    children.push_back(child);
    child->setParent(this);
}

void Object3D::removeChild(Object3D * child){
    for (std::vector<Object3D *>::iterator i = children.begin(); i != children.end(); i++){
        if ((*i) == child){
            children.erase(i);
            break;
        }
    }
}

void Object3D::bindUniformMatrix4f(const GLint handle, glm::mat4 matrix) {
    glUniformMatrix4fv(handle, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Object3D::draw(){
    drawObject();
    for (unsigned int i = 0; i < children.size(); i++){
        children[i]->draw();
    }
}

void Object3D::pushMatrix(){
    matrixStack.push(modelTransform);
}
void Object3D::popMatrix(){
    modelTransform = matrixStack.top();
    matrixStack.pop();
}


void Object3D::reloadTopMatrix(){
    modelTransform = matrixStack.top();
}

void Object3D::resetMatrix(){
    while (matrixStack.size() != 1){
        matrixStack.pop();
    }
    loadIdentity();
}