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
    void loadIdentity();

    void loadArrayBuffer(std::string bufferName, std::vector<float> v);
    void loadElementBuffer(std::vector<unsigned int> v);
    void loadElementBuffer();
    void loadVertexBuffer(std::string bufferName);
    void loadNormalBuffer(std::string bufferName);
    void loadTextureBuffer(std::string bufferName);

    void enableAttrArray3f(std::string aHandle, std::string bufHandle);
    void enableAttrArray2f(std::string aHandle, std::string bufHandle);
    void disableAttrArray(std::string aHandle);
    
    
    void unbindAll();
    
    void bindElements();
    void bindUniformMatrix4f(const GLint handle, glm::mat4 matrix);
    void bindModelMatrix(std::string handle);
    void bindViewMatrix(std::string handle);
    void bindProjectionMatrix(std::string handle);
    
    void addChild(Object3D * child);
    void setParent(Object3D * _parent);

    Mesh * getMesh();
    GLuint getElementBuffer();
    GLuint getArrayBuffer(std::string bufferName);
    glm::mat4 getModelMatrix();

    void draw();
    void drawElements();
    virtual void drawObject() = 0;

protected:
    Mesh * mesh;
    Shader * shader;
    Object3D * parent;

    void pushMatrix();
    void popMatrix();
    void reloadTopMatrix();
    void resetMatrix();

private:
    std::vector<Object3D *> children;

    std::map<std::string, GLuint> arrayBuffers;
    GLuint elementBuffer;

    std::stack <glm::mat4> matrixStack;
    glm::mat4 modelTransform;
};

#endif