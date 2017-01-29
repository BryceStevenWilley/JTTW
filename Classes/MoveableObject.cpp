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
       _maxVelocities(maxVelocities) {}

void MoveableObject::accelerateLeft(float deltaVel) {
    forceXLeft += deltaVel;
    velocities.x = forceXRight - forceXLeft;
}

void MoveableObject::accelerateRight(float deltaVel) {
    forceXRight += deltaVel;
    velocities.x = forceXRight - forceXLeft;
}

void MoveableObject::stop() {
    velocities.x = 0.0;
    forceXRight = 0.0;
    forceXLeft = 0.0;
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
