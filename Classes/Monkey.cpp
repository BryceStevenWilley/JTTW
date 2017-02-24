#include "Monkey.hpp"

using namespace JTTW;

Monkey::Monkey(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Monkey", cocos2d::PhysicsMaterial(1.0, 0.0, 0.0), startPosition, dimensions), _state(NORMAL) {}

void Monkey::impulseLeft(float deltaVel) {
    if (_state != CLIMBING) {
        Character::impulseLeft(deltaVel);
    } else if (this->getScaleX() > 0) {
        // Facing right
        leavingClimeable();
        Character::impulseLeft(deltaVel);
    }
}

void Monkey::impulseRight(float deltaVel) {
    if (_state != CLIMBING) {
        Character::impulseRight(deltaVel);
    } else if (this->getScaleX() < 0) {
        // Facing right
        leavingClimeable();
        Character::impulseRight(deltaVel);
    }
}

void Monkey::jump() {
    float jumpPower = 400;
    if (_state == CLIMBING) {
        std::cout << "Currently Climbing" << std::endl;
        leavingClimeable();
        this->_currentState = Character::State::STANDING;
        jumpPower = 350;
    }
    if (_state == SWINGING) {
        std::cout << "Currently swinging" << std::endl;
        if (j != nullptr) {
            j->removeFormWorld();
            j = nullptr;
        }
        this->_currentState = Character::State::STANDING;
        jumpPower = 100;
        _state = NORMAL;
        //j->setEnable(false);
        //sthis->body(setPositionX(this->body->getPosition())
    }
    Character::jump(jumpPower);
}

void Monkey::characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) {
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
        std::cout << "Toggling climbing" << std::endl;
        // Stick to where you are.
        body->setGravityEnable(false);
        Character::stop();
        body->setVelocity(cocos2d::Vec2::ZERO);
        _state = CLIMBING;
        this->setAnimation(0, "ClimbIdle", true);
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

void Monkey::enteringVine(cocos2d::PhysicsWorld *world, cocos2d::PhysicsBody *vine, double offset, cocos2d::Vec2 collisionPoint) {
    if (_state == NORMAL) {
        if (j != nullptr) {
            j->removeFormWorld();
            j = nullptr;
        }
        // create a joint between you and the vine.
        j = cocos2d::PhysicsJointPin::construct(this->body, vine, cocos2d::Vec2::ZERO, cocos2d::Vec2(0, offset));
        world->addJoint(j);
        _state = SWINGING;
        this->setAnimation(0, "ClimbIdle", true);
    } if (_state == SWINGING) {
        if (j != nullptr) {
            j->removeFormWorld();
            j = nullptr;
        }
        //j->removeFormWorld();
        // create a joint between you and the vine.
        j = cocos2d::PhysicsJointPin::construct(this->body, vine, cocos2d::Vec2::ZERO, cocos2d::Vec2(0, offset));
        world->addJoint(j);
        _state = SWINGING;
        this->setAnimation(0, "ClimbIdle", true);
    }
}
