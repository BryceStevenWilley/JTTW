#include "MoveablePlatform.hpp"
#include "Collisions.hpp"
#include <iostream>

using namespace JTTW;

MoveablePlatform::MoveablePlatform(std::string &fileName, cocos2d::Vec2 centerA, cocos2d::Vec2 centerB, cocos2d::Size imageSize, cocos2d::Vec2 box, double maxVelocity) : image(cocos2d::Sprite::create(fileName)), _centerA(centerA), _centerB(centerB) {
    image->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);

    body = cocos2d::PhysicsBody::createBox(image->getContentSize(), cocos2d::PhysicsMaterial(1.0, 0.0, 0.0));
    body->setDynamic(false); // moving platforms are kinematic bodies.
    body->setGravityEnable(false);
    body->setTag((int)CollisionCategory::Platform);
    body->setContactTestBitmask((int)CollisionCategory::CharacterAndBoulder);
    body->setCollisionBitmask((int)CollisionCategory::CharacterAndBoulder);
     
    image->addComponent(body);

    cocos2d::Size actual = image->getContentSize(); // actual image size.
    image->setScaleX(imageSize.width / actual.width);
    image->setScaleY(imageSize.height / actual.height);

    // Set initial speed.
    _velocities = maxVelocity * (centerB - centerA)/(centerB - centerA).getLength();
    
    image->setPosition(centerA);
}

MoveablePlatform::MoveablePlatform(std::string &fileName, cocos2d::Vec2 centerA, cocos2d::Vec2 centerB, cocos2d::Size imageSize, std::vector<cocos2d::Vec2> points, double maxVelocity) : image(cocos2d::Sprite::create(fileName)), _centerA(centerA), _centerB(centerB) {
    image->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);

    body = cocos2d::PhysicsBody::createPolygon(points.data(), (int)points.size(), cocos2d::PhysicsMaterial(1.0, 0.0, 0.0));
    body->setDynamic(false); // moving platforms are kinematic bodies.
    body->setGravityEnable(false);
    body->setTag((int)CollisionCategory::Platform);
    body->setContactTestBitmask((int)CollisionCategory::CharacterAndBoulder);
    body->setCollisionBitmask((int)CollisionCategory::CharacterAndBoulder);
     
    image->addComponent(body);

    cocos2d::Size actual = image->getContentSize(); // actual image size.
    image->setScaleX(imageSize.width / actual.width);
    image->setScaleY(imageSize.height / actual.height);

    // Set initial speed.
    _velocities = maxVelocity * (centerB - centerA)/(centerB - centerA).getLength();
    
    image->setPosition(centerA);
}


void MoveablePlatform::move(float deltaTime, bool debugOn) {
    auto position = image->getPosition() + _velocities * deltaTime;

    // Use get lengthSq because it saves up 2 square root operations.
    if (currentState == TOWARDS_B && (position - _centerA).getLengthSq() > (_centerB - _centerA).getLengthSq()) {
        // We've gone too far towards B, so let's turn around again.
        _velocities = - _velocities;
    } else if (currentState == TOWARDS_A && (position - _centerB).getLengthSq() > (_centerA - _centerB).getLengthSq()) {
        // We've gone too far towards A, so let's turn around again.
        _velocities = - _velocities;
    }
    body->setVelocity(_velocities);
}

