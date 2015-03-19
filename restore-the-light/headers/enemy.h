#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "GLIncludes.h"

#define degreesToRadians(x) (x*(3.141592f/180.0f))
#define GLM_FORCE_RADIANS
#define GLM_FORCE_PURE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "object3d.h"
#include "game_map.h"
#include "collision_manager.h"

class BodyPart : public Object3D{
public:
    BodyPart(Mesh * m, Shader * s, Object3D * _body) : body(_body){
        mesh = m;
        shader = s;
    }

    void init(){
        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadTextureBuffer("texBufObj");
        loadElementBuffer();

        collisionListPos = CollisionManager::addEnemy(this);
        setCollisionsMask(4, 0);
    }

    void drawObject(){

    }

    void setMatrix(glm::mat4 matrix){
        loadIdentity();
        addTransformation(matrix);
    }

    void onCollision(Object3D * obj){
        body->onCollision(obj);
    }

    void kill(){
        //std::cout << mesh->getFileName() << std::endl;
        CollisionManager::removeEnemy(collisionListPos);
    }

private:
    Object3D * body;
    std::list<Object3D *>::iterator collisionListPos;
};


class Enemy: public Object3D{
public:
    Enemy(GameMap * _gameMap, int _cubePos, float _bodyAngle, glm::vec3 _direction) 
        : gameMap(_gameMap), currentCube(_cubePos), bodyAngle(_bodyAngle), direction(_direction){}

    void init(){
        mesh = LoadManager::getMesh("cube-textures.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadTextureBuffer("texBufObj");
        loadElementBuffer();

        pos = gameMap->getCubePos(currentCube);
        pos.y += 3;

        shader = LoadManager::getShader(
            "geometry-pass-map-texture-vertex.glsl",
            "geometry-pass-map-texture-fragment.glsl");

        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/head.obj") , shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/torso.obj"), shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/leftarm.obj"), shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/rightarm.obj"), shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/lefteyelid.obj"), shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/righteyelid.obj"), shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/leftthigh.obj"), shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/rightthigh.obj"), shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/leftfoot.obj"), shader, this));
        parts.push_back(new BodyPart(LoadManager::getMesh("blue-creature/rightfoot.obj"), shader, this));


        for (int i = 0; i < parts.size(); i++){
            parts[i]->init();
        }

        tex = TextureManager::getTexture("enemy.bmp")->getTexture();
        texShot = TextureManager::getTexture("enemy-shot.bmp")->getTexture();
        atexShot = TextureManager::getTexture("enemy-shot-alpha.bmp")->getTexture();
        atex = TextureManager::getTexture("enemy-alpha.bmp")->getTexture();
    }

    void drawObject(){
        if (removeEnemy)
            return;

        updateAnimation();
        glUseProgram(shader->getId());

        Material::SetMaterial(Material::BLUE_PLASTIC, shader, false, false);

        if (life < 2.0f){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texShot);
            glUniform1i(shader->getHandle("uTexID"), 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, atexShot);
            glUniform1i(shader->getHandle("uAlphaTexID"), 1);

            if (life == 0.0f){
                double timePercent = (glfwGetTime() - deathTime) / howLongToDie;
                if (timePercent >= 1.0){
                    timePercent = 1.0;
                    removeEnemy = true;
                }

                glUniform3f(shader->getHandle("UeColor"), timePercent, timePercent, timePercent);
            }
            else{
                glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);
            }
        }
        else{
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex);
            glUniform1i(shader->getHandle("uTexID"), 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, atex);
            glUniform1i(shader->getHandle("uAlphaTexID"), 1);

            glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);
        }


        glUniform1i(shader->getHandle("uCompleteGlow"), 0);

        bindUniformMatrix4f(
            shader->getHandle("uProjMatrix"),
            CamManager::currentCam()->projectionMatrix);

        bindUniformMatrix4f(
            shader->getHandle("uViewMatrix"),
            CamManager::currentCam()->getViewMatrix());

        loadIdentity();


        //Head
        enablePartArrays(head);

        addTransformation(glm::translate(glm::mat4(1.0f), pos));
        addTransformation(glm::rotate(glm::mat4(1.0f), degreesToRadians(bodyAngle), glm::vec3(0, 1, 0)));
        pushMatrix();
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[head]->setMatrix(getModelMatrix());
        
        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));
        
        parts[head]->drawElements();

        reloadTopMatrix();

        //Left Eye lid
        enablePartArrays(lefteyelid);

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0.17, 0.19, 0.23)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[lefteyelid]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[lefteyelid]->drawElements();

        reloadTopMatrix();


        //Right Eye lid
        enablePartArrays(righteyelid);
        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(-0.23, 0.17, 0.23)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[righteyelid]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[righteyelid]->drawElements();

        reloadTopMatrix();

        //Torso
        enablePartArrays(torso);

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(-0.02, -0.581, -0.15)));
        pushMatrix();
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[torso]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[torso]->drawElements();

        reloadTopMatrix();

        //Left Arm
        enablePartArrays(leftarm);

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0.83, -0.12, -0.11)));
        addTransformation(glm::rotate(glm::mat4(1.0f), degreesToRadians(rightArmAngle), glm::vec3(-1, 0, 0)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[leftarm]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[leftarm]->drawElements();

        reloadTopMatrix();

        //Right Arm
        enablePartArrays(rightarm);

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(-0.69, -0.08, -0.04)));
        addTransformation(glm::rotate(glm::mat4(1.0f), degreesToRadians(leftArmAngle), glm::vec3(-1, 0, 0)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[rightarm]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[rightarm]->drawElements();

        reloadTopMatrix();


        //Left Thigh
        enablePartArrays(leftthigh);

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0.35, -0.61, 0)));
        pushMatrix();
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[leftthigh]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[leftthigh]->drawElements();

        reloadTopMatrix();

        //Left Foot
        enablePartArrays(leftfoot);

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0.19, -0.74, 0.27)));
        addTransformation(glm::rotate(glm::mat4(1.0f), degreesToRadians(rightFootAngle), glm::vec3(-1, 0, 0)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[leftfoot]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[leftfoot]->drawElements();

        reloadTopMatrix();


        //Right Thigh
        enablePartArrays(rightthigh);

        popMatrix();
        reloadTopMatrix();

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(-0.32, -0.62, -0.03)));
        pushMatrix();
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[rightthigh]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[rightthigh]->drawElements();

        reloadTopMatrix();

        //Right Foot
        enablePartArrays(rightfoot);

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(-0.19, -0.74, 0.27)));
        addTransformation(glm::rotate(glm::mat4(1.0f), degreesToRadians(leftFootAngle), glm::vec3(-1, 0, 0)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.2)));
        parts[rightfoot]->setMatrix(getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uModelMatrix"),
            getModelMatrix());

        bindUniformMatrix4f(
            shader->getHandle("uNormalMatrix"),
            glm::transpose(glm::inverse(CamManager::currentCam()->getViewMatrix()*getModelMatrix())));

        parts[rightfoot]->drawElements();

        reloadTopMatrix();


        disableAttrArray("aPosition");
        disableAttrArray("aNormal");
        disableAttrArray("aTexCoord");
        unbindAll();

        if (removeEnemy){
            if (parent != NULL)
                parent->removeChild(this);
        }
    }

    void onCollision(Object3D *obj){
        if (finished)
            return;

        
        if (obj->getCollisionTypeMask() == 3 && differentShot(obj)){
            shotHit = obj;
            life -= 1.0f;
            if (life == 0.0f){
                kill();
            }
        }
    }

    void kill(){
        if (finished)
            return;

        for (int i = 0; i < parts.size(); i++){
            parts[i]->kill();
        }
        deathTime = glfwGetTime();
        finished = true;
    }

    void updateAnimation(){
        float armVelocity = maxArmAngle*stepVelocity / maxFeetAngle;

        if (rightFootMoving){
            rightFootAngle += feetDirection*stepVelocity;
            leftArmAngle += feetDirection*armVelocity;
            
            if (rightFootAngle <= 0){
                rightFootAngle = 0;
                leftArmAngle = 0;
                
                rightFootMoving = false;
                leftFootMoving = true;

                feetDirection = 1;
            }
            else if (rightFootAngle >= maxFeetAngle){
                rightFootAngle = maxFeetAngle;
                leftArmAngle = maxArmAngle;
                feetDirection = -1;
            }
        }
        else{
            leftFootAngle += feetDirection*stepVelocity;
            rightArmAngle += feetDirection*armVelocity;

            if (leftFootAngle <= 0){
                leftFootAngle = 0;
                rightArmAngle = 0;

                rightFootMoving = true;
                leftFootMoving = false;

                feetDirection = 1;
            }
            else if (leftFootAngle >= maxFeetAngle){
                leftFootAngle = maxFeetAngle;
                rightArmAngle = maxArmAngle;
                feetDirection = -1;
            }
        }
    }


    bool isDead(){
        return removeEnemy;
    }

private:
    GameMap * gameMap;
    int currentCube;
    std::vector<BodyPart *> parts;
    bool finished = false;
    double deathTime, howLongToDie = 1.0;
    float life = 2;
    bool removeEnemy = false;

    glm::vec3 pos, direction;

    GLuint tex, atex, texShot, atexShot;

    float bodyAngle = 0;
    bool rightFootMoving = true, leftFootMoving = false;
    float rightFootAngle = 0, leftFootAngle = 0;
    float rightArmAngle = 0, leftArmAngle = 0;
    float feetDirection = 1, maxFeetAngle = 30.0f, maxArmAngle = 85.0f, stepVelocity = 0.8f;

    const int head = 0, torso = 1, leftarm = 2, rightarm = 3,
        lefteyelid = 4, righteyelid = 5, leftthigh = 6, rightthigh = 7,
        leftfoot = 8, rightfoot = 9;

    Object3D * shotHit = NULL;

    bool differentShot(Object3D * obj){
        return shotHit != obj;
    }

    void enablePartArrays(int part){
        parts[part]->enableAttrArray3f("aPosition", "posBufObj");
        parts[part]->enableAttrArray3f("aNormal", "norBufObj");
        parts[part]->enableAttrArray3f("aTexCoord", "texBufObj");
        parts[part]->bindElements();
    }

};

#endif
