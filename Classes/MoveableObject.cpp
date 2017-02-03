//
//  MoveableObject.cpp
//  JTTW
//
//  Created by Bryce Willey on 1/28/17.
//
//

#include "MoveableObject.hpp"

using namespace JTTW;

MoveableObject::MoveableObject(JTTW::Rectangle dimensions, cocos2d::Vec2 maxVelocities) : GameObject(dimensions),
       _maxVelocities(maxVelocities) {
    physics = cocos2d::PhysicsBody::createBox(cocos2d::Size(dimensions.getWidth(), dimensions.getHeight()));
    physics->setDynamic(true); // TODO: change?
    physics->setTag(1);
    physics->setContactTestBitmask(0xFFFFFFFF);
    physics->setVelocityLimit(500);
}

void MoveableObject::accelerateLeft(float deltaVel) {
    //forceXLeft += deltaVel;
    //velocities.x = forceXRight - forceXLeft;
    physics->setVelocity(cocos2d::Vec2(-100, 0));
    //physics->
    //physics->setVelocity(cocos2d::Vec2(velocities.x * _maxVelocities.x, velocities.y * _maxVelocities.y));
}

void MoveableObject::accelerateRight(float deltaVel) {
    //forceXRight += deltaVel;
    //velocities.x = forceXRight - forceXLeft;
    physics->setVelocity(cocos2d::Vec2(100, 0));
   // physics->setVelocity(cocos2d::Vec2(velocities.x * _maxVelocities.x, velocities.y * _maxVelocities.y));
}

void MoveableObject::stop() {
    //velocities.x = 0.0;
    //forceXRight = 0.0;
    //forceXLeft = 0.0;
    physics->setVelocity(cocos2d::Vec2(0,0));
}

bool MoveableObject::isMovingLeft() const {
    return velocities.x < 0.0;
}

bool MoveableObject::isMovingRight() const {
    return velocities.x > 0.0;
}

double MoveableObject::getXVelocity() const {
    return velocities.x * _maxVelocities.x;
}

double MoveableObject::getYVelocity() const {
    return velocities.y * _maxVelocities.y;
}
