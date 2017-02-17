//
//  Monkey.cpp
//  JTTW
//
//  Created by Bryce Willey on 2/15/17.
//
//

#include "Monkey.hpp"

using namespace JTTW;

Monkey::Monkey(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Monkey", cocos2d::PhysicsMaterial(1.0, 0.0, 1.0), startPosition, dimensions) {}

void Monkey::impulseLeft(float deltaVel) {
    if (_state != CLIMBING) {
        Character::impulseLeft(deltaVel);
    }
}

void Monkey::impulseRight(float deltaVel) {
    if (_state != CLIMBING) {
        Character::impulseRight(deltaVel);
    }
}

void Monkey::jump() {
    if (_state == CLIMBING) {
        leavingClimeable();
        this->_currentState = Character::State::STANDING;
    }
    Character::jump(400);
}

void Monkey::characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) {
    if (code == cocos2d::EventKeyboard::KeyCode::KEY_S && pressed) {
        std::cout << "Toggling climbing" << std::endl;
        if (_state == COULD_CLIMB) {
            // Stick to where you are.
            body->setGravityEnable(false);
            Character::stop();
            body->setVelocity(cocos2d::Vec2::ZERO);
            _state = CLIMBING;
            this->setAnimation(0, "ClimbIdle", true);
        } else if (_state == CLIMBING) {
            body->setGravityEnable(true);
            _state = COULD_CLIMB;
            this->setAnimation(0, "JumpBackFromClimb", false);
        }
    }
    
    if (code == cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW && _state == CLIMBING) {
        if (pressed) {
            climbUpVel += 200;
        } else {
            climbUpVel -= 200;
        }
        updateClimbingVel();
    }
    
    if (code == cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW && _state == CLIMBING) {
        if (pressed) {
            climbDownVel += 200;
        } else {
            climbDownVel -= 200;
        }
        updateClimbingVel();
    }
}

void Monkey::updateClimbingVel() {
    double sum = climbUpVel - climbDownVel;
    if (sum != 0.0) {
        this->setTimeScale(2.0);
        this->setAnimation(0, "Climb", true);
    } else {
        this->setTimeScale(1.0);
        this->setAnimation(0, "ClimbIdle", true);
    }
    body->setVelocity(cocos2d::Vec2(0, sum));
}

void Monkey::enteringClimeable() {
    if (_state == NORMAL) {
        _state = COULD_CLIMB;
    }
}

void Monkey::leavingClimeable() {
    if (_state == CLIMBING) {
        this->setTimeScale(1.0);
        this->setAnimation(0, "JumpForwardFromClimb", false);
    }
    _state = NORMAL;
    climbUpVel = 0.0;
    climbDownVel = 0.0;
    body->setGravityEnable(true);
}
