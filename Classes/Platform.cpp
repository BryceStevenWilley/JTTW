#include "Platform.hpp"
#include "Collisions.hpp"

using namespace JTTW;

Platform::Platform() : cocos2d::Sprite() {
    this->init();
}

Platform::Platform(std::string &fileName, cocos2d::Vec2 center, cocos2d::Size imageSize, cocos2d::Vec2 box, bool climeable, bool collidable) : cocos2d::Sprite() {
    if (!this->initWithFile(fileName)) {
        throw std::invalid_argument(fileName);
    }
    this->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    this->setPosition(center);
    this->setContentSize(imageSize);
    
    if (collidable) {
        body = cocos2d::PhysicsBody::createBox(cocos2d::Size(box.x, box.y), cocos2d::PhysicsMaterial(1.0, 0.0, 1.0));
        body->setDynamic(false);
        body->setGravityEnable(false);
        body->setTag((int)CollisionCategory::Platform);
        body->setContactTestBitmask((int)CollisionCategory::CharacterAndBoulder);
        body->setCollisionBitmask((int)CollisionCategory::CharacterAndBoulder);
    
        this->addComponent(body);
    }

    if (climeable) {
        this->setTag(CLIMBEABLE_TAG);
    }
}
