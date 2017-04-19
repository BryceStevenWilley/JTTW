#include "Dragon.hpp"
#include "Collisions.hpp"
#include "cocos2d.h"

using namespace JTTW;

Dragon::Dragon(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) {
    
    body = cocos2d::PhysicsBody::createBox(cocos2d::Size(dimensions.width / 2.0, 700), cocos2d::PhysicsMaterial(1.0, 1.0, 1.0));
    
    body->setCategoryBitmask((int)CollisionCategory::Platform);
    body->setCollisionBitmask((int)CollisionCategory::CharacterPlatformAndBoulder);
    body->setContactTestBitmask((int)CollisionCategory::ALL);
    body->setDynamic(false);

    this->initWithJsonFile("characters/dragon.json", "characters/dragon.atlas", 1.0f);
    this->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    this->setScaleX(dimensions.width / 1400.0);
    this->setScaleY(dimensions.height / 700.0);
    this->setAnimation(0, "idle", true);
    this->addComponent(body);
    
    this->setPosition(startPosition);
}

void Dragon::move(cocos2d::Vec2 vel) {
    body->setVelocity(vel);
    if (vel.x > 0) {
        // Dragon's default facing direction is left.
        this->setScaleX(-1 * std::abs(this->getScaleX()));
    } else {
        this->setScaleX(std::abs(this->getScaleX()));
    }
}
