#include "Spear.hpp"
#include "SimpleAudioEngine.h"
#include "Collisions.hpp"

using namespace JTTW;

Spear::Spear(cocos2d::Vec2 targetCenter) : cocos2d::Sprite() {
    this->initWithFile("spear.png");
    // Throw a projectile somewhere!
    auto body = cocos2d::PhysicsBody::createBox(cocos2d::Size(300, 50));
    body->setDynamic(true);
    body->setGravityEnable(true);
    body->setTag((int)CollisionCategory::Projectile);
    body->setCollisionBitmask((int)CollisionCategory::None);
    body->setContactTestBitmask((int)CollisionCategory::Character);
    body->setVelocity(cocos2d::Vec2(1000, 100));
    this->setPosition(targetCenter.x - 500, targetCenter.y + 100);
    this->setContentSize(cocos2d::Size(300, 50));
    this->addComponent(body);
    this->setTag(PROJECTILE_TAG);
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Sound/Spear.wav");
}
