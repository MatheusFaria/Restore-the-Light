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
    //std::cout << "Begin: Checking Collision with ground...\n";
    for (std::list<Object3D *>::iterator it = ground.begin(); it != ground.end(); it++){
        (*it)->checkCollision(obj);
    }
    //std::cout << "End: Checking Collision with groun...\n";
}



std::list<Object3D *>::iterator CollisionManager::addEnemy(Object3D * enemy){
    enemies.push_front(enemy);
    return enemies.begin();
}

void CollisionManager::removeEnemy(std::list<Object3D *>::iterator enemy){
    //std::cout << "Begin: Removing Enemy...\n";
    enemies.erase(enemy);
    //std::cout << "End: Removing Enemy...\n";
}

void CollisionManager::checkCollisionWithEnemy(Object3D * obj){
    //std::cout << "Begin: Checking Collision with enemy...\n";
    std::list<Object3D *> bkp = enemies;
    for (std::list<Object3D *>::iterator it = bkp.begin(); it != bkp.end(); it++){
        (*it)->checkCollision(obj);
    }
    //std::cout << "End: Checking Collision with enemy...\n";
}



std::list<Object3D *>::iterator CollisionManager::addShot(Object3D * shot){
    shots.push_front(shot);
    return shots.begin();
}

void CollisionManager::removeShot(std::list<Object3D *>::iterator shot){
    //std::cout << "Begin: Removing shot...\n";
    shots.erase(shot);
    //std::cout << "End: Removing shot...\n";
}

void CollisionManager::checkCollisionWithShot(Object3D * obj){
    //std::cout << "Begin: Checking Collision with shot...\n";
    std::list<Object3D *> bkp = shots;
    for (std::list<Object3D *>::iterator it = bkp.begin(); it != bkp.end(); it++){
        (*it)->checkCollision(obj);
    }
    //std::cout << "End: Checking Collision with shot...\n";
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