#include "CageTrap.hpp"
#include "Collisions.hpp"
#include "SimpleAudioEngine.h"

using namespace JTTW;

CageTrap::CageTrap(std::string imageName, cocos2d::Vec2 center, cocos2d::PhysicsMaterial material, 
        cocos2d::Size trapSize, cocos2d::Size imageSize, double wallThickness, double offset) :
        cocos2d::Sprite() {
    this->initWithFile(imageName);
    this->setPosition(center);
    this->setContentSize(imageSize);

    auto left = cocos2d::PhysicsShapeBox::create(cocos2d::Size(wallThickness, trapSize.height * 5.0/6.0), material, cocos2d::Vec2(-trapSize.width/2.0, offset - 1.0/12 * trapSize.height));
    auto right = cocos2d::PhysicsShapeBox::create(cocos2d::Size(wallThickness, trapSize.height * 5.0/6.0), material, cocos2d::Vec2(trapSize.width/2.0, offset - 1.0/12 * trapSize.height));
    auto top = cocos2d::PhysicsShapeBox::create(cocos2d::Size(trapSize.width / 2, wallThickness), material, cocos2d::Vec2(0, trapSize.height/1.7 + offset));
    auto leftTop = cocos2d::PhysicsShapeBox::create(cocos2d::Size(trapSize.width / 3.7, wallThickness), material, cocos2d::Vec2(-trapSize.width/2.5, trapSize.height/3.0 + offset));
    auto rightTop = cocos2d::PhysicsShapeBox::create(cocos2d::Size(trapSize.width / 3.7, wallThickness), material, cocos2d::Vec2(trapSize.width/2.5, trapSize.height/3.0 + offset));
    auto topLeft = cocos2d::PhysicsShapeBox::create(cocos2d::Size(wallThickness, trapSize.height / 3.5), material, cocos2d::Vec2(-trapSize.width/4.0, trapSize.height/2.2 + offset));
    auto topRight = cocos2d::PhysicsShapeBox::create(cocos2d::Size(wallThickness, trapSize.height / 3.5), material, cocos2d::Vec2(trapSize.width/4.0, trapSize.height/2.2 + offset));
    body = cocos2d::PhysicsBody::create();
    body->addShape(left);
    body->addShape(right);
    body->addShape(top);
    body->addShape(leftTop);
    body->addShape(rightTop);
    body->addShape(topLeft);
    body->addShape(topRight);

    body->setCategoryBitmask((int)CollisionCategory::Boulder);
    body->setCollisionBitmask((int)CollisionCategory::CharacterPlatformAndBoulder);
    body->setContactTestBitmask((int)CollisionCategory::CharacterPlatformAndBoulder);
    body->setRotationEnable(true);
    body->setGravityEnable(false);
        
    body->setVelocityLimit(600);
    body->setDynamic(true);
            
    this->addComponent(body);
    this->setVisible(false);
    
    minX = center.x - trapSize.width / 2.0;
    maxX = center.x + trapSize.width / 2.0;
    minY = center.y - trapSize.height / 2.0;
}

bool CageTrap::triggerTrap(cocos2d::Vec2 characterCenter, cocos2d::Size characterSize) {
    if (characterCenter.x + characterSize.width / 2.0 < maxX && characterCenter.x - characterSize.width / 2.0 > minX && characterCenter.y < minY) {
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Sound/CageRelease.wav");
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Sound/CageFall.wav");
        body->setGravityEnable(true);
        this->setVisible(true);
        return true;
    }
    return false;
}

Action CageTrap::characterReaction() const {
    return Action::FREEZE;
}
