#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <map>
#include <list>
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
    
    void addChild(Object3D * child);
    void removeChild(Object3D * child);
    void setParent(Object3D * _parent);

    Mesh * getMesh();
    GLuint getElementBuffer();
    GLuint getArrayBuffer(std::string bufferName);
    glm::mat4 getModelMatrix();

    glm::vec3 getMinPoint();
    glm::vec3 getMaxPoint();
    int getCollisionTypeMask();
    int getCollideWithMask();
    void setCollisionsMask(int typeMask, int collideMask);

    bool collide(Object3D * obj);
    void checkCollision(Object3D * obj);
    virtual void onCollision(Object3D * obj) = 0;

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
    std::list<Object3D *> children;

    std::map<std::string, GLuint> arrayBuffers;
    GLuint elementBuffer;

    std::stack <glm::mat4> matrixStack;
    glm::mat4 modelTransform;

    int collisionTypeMask, collideWithMask;
};

#endif