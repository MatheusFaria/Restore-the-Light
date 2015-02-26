#ifndef OBJECT3D_H
#define OBJECT3D_H

/*
* Author: Matheus de Sousa Faria
* CPE 471 - Introduction to Computer Graphics
* Program 3
*/

#include <map>
#include <vector>
#include <stack>

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
    void enableAttrArray3f(std::string aHandle, std::string bufHandle);
    void disableAttrArray(std::string aHandle);
    void loadIdentity();
    void unbindAll();
    void bindElements();
    void drawElements();
    void addChild(Object3D * child);
    void setParent(Object3D * _parent);
    void bindUniformMatrix4f(const GLint handle, glm::mat4 matrix);
    void bindModelMatrix(std::string handle);
    void bindViewMatrix(std::string handle);
    void bindProjectionMatrix(std::string handle);
    Mesh * getMesh();
    GLuint getElementBuffer();
    GLuint getArrayBuffer(std::string bufferName);
    glm::mat4 getModelMatrix();

    void draw();

    virtual void drawObject() = 0;

protected:
    Mesh * mesh;
    Shader * shader;

    void pushMatrix();
    void popMatrix();
    void reloadTopMatrix();
    void resetMatrix();

private:
    Object3D * parent;
    std::vector<Object3D *> children;

    std::map<std::string, GLuint> arrayBuffers;
    GLuint elementBuffer;

    std::stack <glm::mat4> matrixStack;
    glm::mat4 modelTransform;
};

#endif