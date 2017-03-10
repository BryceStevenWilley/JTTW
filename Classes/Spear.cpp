#include "Spear.hpp"
#include "SimpleAudioEngine.h"
#include "Collisions.hpp"

using namespace JTTW;

SpearRNG Spear::gen;

cocos2d::Vec2 SpearRNG::getVelocity(cocos2d::Vec2 spear, cocos2d::Vec2 target) {
    return cocos2d::Vec2(xVelGen(rng), yVelGen(rng));
}

cocos2d::Vec2 SpearRNG::getPosition(cocos2d::Vec2 target) {
    // always 500 x back, randomize the y offset.
    double xOffset = -500;
    double yOffset = (rand() % 300);
    return cocos2d::Vec2(target.x + xOffset, target.y + yOffset);
}

// TODO: figure out how to rotate it correctly.
Spear::Spear(cocos2d::Vec2 targetCenter) : cocos2d::Sprite() {
    this->initWithFile("assets/spear.png");
    // Throw a projectile somewhere!
    auto body = cocos2d::PhysicsBody::createBox(cocos2d::Size(300, 40));
    body->setDynamic(true);
    
    body->setGravityEnable(true);
    body->setTag((int)CollisionCategory::Projectile);
    body->setCollisionBitmask((int)CollisionCategory::None);
    body->setContactTestBitmask((int)CollisionCategory::Character);
    
    cocos2d::Vec2 pos = gen.getPosition(targetCenter);
    body->setVelocity(gen.getVelocity(pos, targetCenter));
    this->setPosition(pos);
    this->setContentSize(cocos2d::Size(300, 50));
    this->addComponent(body);
    this->setTag(PROJECTILE_TAG);

    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Sound/Spear.wav");
}
