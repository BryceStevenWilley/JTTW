#include "MoveablePlatform.hpp"
#include "Collisions.hpp"
#include <iostream>

using namespace JTTW;

const bool NOT_CLIMABLE = false;
const bool COLLIDABLE = true;

MoveablePlatform::MoveablePlatform(std::string &fileName, cocos2d::Vec2 centerA, cocos2d::Vec2 centerB, cocos2d::Size imageSize, cocos2d::Vec2 box, double maxVelocity) : Platform(fileName, centerA, imageSize, box, NOT_CLIMABLE, COLLIDABLE), _centerA(centerA), _centerB(centerB) {
    // Set initial speed.
    _velocities = maxVelocity * (centerB - centerA)/(centerB - centerA).getLength();
    body->setVelocity(_velocities);
    this->setTag(MOVEABLE_TAG);
    // NOTE: body properties are set in the parent class, Platform.
}

MoveablePlatform::MoveablePlatform(std::string &fileName, cocos2d::Vec2 centerA, cocos2d::Vec2 centerB, cocos2d::Size imageSize, std::vector<cocos2d::Vec2> points, double maxVelocity) : Platform(fileName, centerA, imageSize, points, NOT_CLIMABLE, COLLIDABLE), _centerA(centerA), _centerB(centerB) {

    // Set initial speed.
    _velocities = maxVelocity * (centerB - centerA)/(centerB - centerA).getLength();
    body->setVelocity(_velocities);
    this->setTag(MOVEABLE_TAG);
}

// TODO: Make platforms gradually move
// Few Choices: Spring based
//    Based on a Sin wave
//    Based on a control thing with low may force applied.
void MoveablePlatform::move(float deltaTime, bool debugOn) {
    auto position = this->getPosition() + _velocities * deltaTime;
    
    // Use get lengthSq because it saves up 2 square root operations.
    if (currentState == TOWARDS_B && (position - _centerA).getLengthSq() > (_centerB - _centerA).getLengthSq()) {
        // We've gone too far towards B, so let's turn around again.
        _velocities = - _velocities;
        currentState = TOWARDS_A;
    } else if (currentState == TOWARDS_A && (position - _centerB).getLengthSq() > (_centerA - _centerB).getLengthSq()) {
        // We've gone too far towards A, so let's turn around again.
        _velocities = - _velocities;
        currentState = TOWARDS_B;
    }
    body->setVelocity(_velocities);
}

