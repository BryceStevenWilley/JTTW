#include "MoveablePlatform.hpp"
#include "Collisions.hpp"
#include <iostream>

using namespace JTTW;

MoveablePlatform::MoveablePlatform(std::string &fileName, cocos2d::Vec2 centerA, cocos2d::Vec2 centerB, 
                                   cocos2d::Size imageSize,
                                   std::vector<cocos2d::Vec2> points,
                                   std::vector<double> frictions,
                                   double maxVelocity) 
        : Moveable(fileName, centerA, centerB, imageSize, points, frictions, std::vector<bool>(), maxVelocity), _maxVelocity(maxVelocity) {
    // Set initial speed.
    this->setTag(MOVEABLE_TAG);
    // NOTE: body properties are set in the parent class, Moveable.
}

void MoveablePlatform::move(float deltaTime) {
    // Use get lengthSq because it saves up 2 square root operations.
    if (currentState == TOWARDS_B && goneFarToB(deltaTime)) {
        // We've gone too far towards B, so let's turn around again.
        setVelocityTowardsA(_maxVelocity);
        currentState = TOWARDS_A;
    } else if (currentState == TOWARDS_A && goneFarToA(deltaTime)) {
        // We've gone too far towards A, so let's turn around again.
        setVelocityTowardsB(_maxVelocity);
        currentState = TOWARDS_B;
    }
}
