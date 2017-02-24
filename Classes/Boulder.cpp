#include "Boulder.hpp"
#include "Collisions.hpp"

using namespace JTTW;

Boulder::Boulder(double radius, std::string imageName, cocos2d::Vec2 center, double mass) :
        cocos2d::Sprite() {
    this->initWithFile(imageName);
    
    cocos2d::PhysicsBody *body = cocos2d::PhysicsBody::createCircle(radius); //, cocos2d::PhysicsMaterial(1.0, 0.0, 1.0));
    body->setCategoryBitmask((int)CollisionCategory::Boulder);
    body->setCollisionBitmask((int)CollisionCategory::ALL);
    body->setContactTestBitmask((int)CollisionCategory::ALL);
    body->setRotationEnable(true);
    body->setVelocityLimit(600);
    body->setDynamic(true);
    body->setMass(mass);
    
    this->setPosition(center);
    this->setPhysicsBody(body);
}

// TODO: don't know how to handle image sizes yet. 
Boulder::Boulder(std::vector<cocos2d::Vec2> points, std::string imageName, cocos2d::Vec2 center, double mass) : cocos2d::Sprite() {
    this->initWithFile(imageName);
    
    cocos2d::PhysicsBody *body = cocos2d::PhysicsBody::createPolygon(points.data(), (int)points.size());
    body->setCategoryBitmask((int)CollisionCategory::Boulder);
    body->setCollisionBitmask((int)CollisionCategory::ALL);
    body->setContactTestBitmask((int)CollisionCategory::ALL);
    body->setRotationEnable(true);
    body->setVelocityLimit(600);
    body->setDynamic(true);
    body->setMass(mass);
    
    this->setPosition(center);
    this->setPhysicsBody(body);
}
