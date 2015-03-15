#include "collision_manager.h"

#include <iostream>

std::list< Object3D * > CollisionManager::ground;
std::list< Object3D * > CollisionManager::enemies;
std::list< Object3D * > CollisionManager::shots;
Object3D * CollisionManager::hero, * CollisionManager::boss;

std::list<Object3D *>::iterator CollisionManager::addGround(Object3D * groundPiece){
    ground.push_front(groundPiece);
    return ground.begin();
}

void CollisionManager::removeGround(std::list<Object3D *>::iterator groundPiece){
    ground.erase(groundPiece);
}

void CollisionManager::checkCollisionWithGround(Object3D * obj){
    for (std::list<Object3D *>::iterator it = ground.begin(); it != ground.end(); it++){
        (*it)->checkCollision(obj);
    }
}



std::list<Object3D *>::iterator CollisionManager::addEnemy(Object3D * enemy){
    enemies.push_front(enemy);
    return enemies.begin();
}

void CollisionManager::removeEnemy(std::list<Object3D *>::iterator enemy){
    enemies.erase(enemy);
}

void CollisionManager::checkCollisionWithEnemy(Object3D * obj){
    std::list<Object3D *> bkp = enemies;
    for (std::list<Object3D *>::iterator it = bkp.begin(); it != bkp.end(); it++){
        (*it)->checkCollision(obj);
    }
}



std::list<Object3D *>::iterator CollisionManager::addShot(Object3D * shot){
    shots.push_front(shot);
    return shots.begin();
}

void CollisionManager::removeShot(std::list<Object3D *>::iterator shot){
    shots.erase(shot);
}

void CollisionManager::checkCollisionWithShot(Object3D * obj){
    for (std::list<Object3D *>::iterator it = shots.begin(); it != shots.end(); it++){
        (*it)->checkCollision(obj);
    }
}



void CollisionManager::setHero(Object3D * _hero){
    hero = _hero;
}

Object3D * CollisionManager::getHero(){
    return hero;
}

void CollisionManager::checkCollisionWithHero(Object3D * obj){
    obj->collide(hero);
}



void CollisionManager::setBoss(Object3D * _boss){
    boss = _boss;
}

Object3D * CollisionManager::getBoss(){
    return boss;
}

void CollisionManager::checkCollisionWithBoss(Object3D * obj){
    obj->collide(boss);
}