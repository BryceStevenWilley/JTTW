#include "Vine.hpp"
#include "Collisions.hpp"
#include "Resolution.hpp"

using namespace JTTW;

Vine::Vine(std::string fileName, cocos2d::Vec2 rotCenter, double width, double length, double startingAngVel) :
     SceneObject(), rotCenter(rotCenter), _length(ideal2Res(length)) {
    
    this->initWithFile(fileName);
    cocos2d::Size imageSize = cocos2d::Size(width, length);
    cocos2d::Vec2 box = cocos2d::Vec2(width, length);
    cocos2d::Vec2 center = rotCenter - cocos2d::Vec2(0, length / 2.0);
    this->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    this->setPosition(center);
    this->setContentSize(imageSize);
    
    body = cocos2d::PhysicsBody::createBox(cocos2d::Size(box.x, box.y), cocos2d::PhysicsMaterial(0.2, 0.0, 0.0));
    
    body->setDynamic(true);
    body->setGravityEnable(true);
    body->setRotationEnable(true);
    body->setCollisionBitmask((int)CollisionCategory::None);
    body->setContactTestBitmask((int)CollisionCategory::Character);
    body->setAngularVelocity(startingAngVel);

    this->addComponent(body);
    this->setTag(VINE_TAG);
}

cocos2d::PhysicsBody *Vine::getBody() {
    return body;
}

cocos2d::Vec2 Vine::getRotationCenter() {
    return rotCenter;
}

double Vine::getLength() const {
    return _length;
}

double Vine::getYRange() {
    return getLength();
}
