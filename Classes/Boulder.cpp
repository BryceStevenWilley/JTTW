#include "Boulder.hpp"
#include "Collisions.hpp"
#include "Resolution.hpp"

using namespace JTTW;

const double BOULDER_VELOCITY_LIMIT = ideal2Res(600);

Boulder::Boulder(double radius, std::string imageName, cocos2d::Vec2 center, double mass, cocos2d::Size imgSize) :
        cocos2d::Sprite() {
    this->initWithFile(imageName);
    
    body = cocos2d::PhysicsBody::createCircle(radius); //, cocos2d::PhysicsMaterial(1.0, 0.0, 1.0));
    body->setCategoryBitmask((int)CollisionCategory::Boulder);
    body->setCollisionBitmask((int)CollisionCategory::CharacterPlatformAndBoulder);
    body->setContactTestBitmask((int)CollisionCategory::CharacterPlatformAndBoulder);
    body->setGravityEnable(true);
    body->setRotationEnable(true);
    body->setVelocityLimit(BOULDER_VELOCITY_LIMIT);
    body->setDynamic(true);
    body->setMass(mass);
    
    this->setPosition(center);
    this->setContentSize(imgSize);
    this->setPhysicsBody(body);
}

Boulder::Boulder(std::vector<cocos2d::Vec2> points, std::string imageName, cocos2d::Vec2 center, double mass, cocos2d::Size imgSize) : cocos2d::Sprite() {
    this->initWithFile(imageName);
    
    body = cocos2d::PhysicsBody::createPolygon(points.data(), (int)points.size());
    body->setCategoryBitmask((int)CollisionCategory::Boulder);
    body->setCollisionBitmask((int)CollisionCategory::CharacterPlatformAndBoulder);
    body->setContactTestBitmask((int)CollisionCategory::CharacterPlatformAndBoulder);
    body->setGravityEnable(true);
    body->setRotationEnable(true);
    body->setVelocityLimit(BOULDER_VELOCITY_LIMIT);
    body->setDynamic(true);
    body->setMass(mass);
    body->setVelocity(cocos2d::Vec2::ZERO);
    
    this->setPosition(center);
    this->setContentSize(imgSize);
    this->setPhysicsBody(body);
}

cocos2d::PhysicsBody *Boulder::getBody() {
    return body;
}
