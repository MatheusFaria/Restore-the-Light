#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include <list>

#include "object3d.h"

class CollisionManager{
public:
    static std::list<Object3D *>::iterator addGround(Object3D * groundPiece);
    static void removeGround(std::list<Object3D *>::iterator groundPiece);
    static void checkCollisionWithGround(Object3D * obj);

    static std::list<Object3D *>::iterator addEnemy(Object3D * enemy);
    static void removeEnemy(std::list<Object3D *>::iterator enemy);
    static void checkCollisionWithEnemy(Object3D * obj);

    static std::list<Object3D *>::iterator addShot(Object3D * shot);
    static void removeShot(std::list<Object3D *>::iterator shot);
    static void checkCollisionWithShot(Object3D * obj);

    static void setHero(Object3D * _hero);
    static Object3D * getHero();
    static void checkCollisionWithHero(Object3D * obj);

    static void setBoss(Object3D * _boss);
    static Object3D * getBoss();
    static void checkCollisionWithBoss(Object3D * obj);

private:
    static std::list< Object3D * > ground;
    static std::list< Object3D * > enemies;
    static std::list< Object3D * > shots;
    static Object3D * hero, *boss;
};

#endif