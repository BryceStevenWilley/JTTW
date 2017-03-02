#include "Monkey.hpp"

using namespace JTTW;

Monkey::Monkey(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Monkey", cocos2d::PhysicsMaterial(1.0, 0.0, 0.0), startPosition, dimensions), _state(NORMAL) {}

void Monkey::impulseLeft(float deltaVel) {
    if (_state == NORMAL) {
        Character::impulseLeft(deltaVel);
    } else if (_state == SWINGING) {
        Character::impulseLeft(deltaVel); // so you don't have to swing as much to get momentum.
    } else if (this->getScaleX() > 0) { // _state == CLIMBING
        // Facing right
        leavingClimeable();
        Character::impulseLeft(deltaVel);
    } else if (this->getScaleX() < 0  && deltaVel > 0) {
        // Facing left, towards the obstacle.
        Character::impulseLeftButNoRebalance(deltaVel);
    }
}

void Monkey::impulseRight(float deltaVel) {
    if (_state == NORMAL) {
        Character::impulseRight(deltaVel);
    } else if (_state == SWINGING) {
        Character::impulseRight(deltaVel);
    } else if (this->getScaleX() < 0) { // _state == CLIMBING
        // Facing right
        leavingClimeable();
        Character::impulseRight(deltaVel);
    } else if (this->getScaleX() > 0 && deltaVel > 0) {
        // Facing right, towards the obstacle.
        Character::impulseRightButNoRebalance(deltaVel);
    }
}

void Monkey::jump() {
    float jumpPower = 500;
    if (_state == CLIMBING) {
        std::cout << "Currently Climbing" << std::endl;
        leavingClimeable();
        this->_currentState = Character::State::STANDING;
        jumpPower = 400;
    }
    if (_state == SWINGING) {
        std::cout << "Currently swinging" << std::endl;
        if (pinJoint != nullptr) {
            pinJoint->removeFormWorld();
            pinJoint = nullptr;
        }
        
        if (gearJoint != nullptr) {
            gearJoint->removeFormWorld();
            gearJoint = nullptr;
        }
        
        this->_currentState = Character::State::STANDING;
        jumpPower = 90;
        _state = NORMAL;
        //Character::jump(jumpPower);
        cocos2d::Vec2 vel = body->getVelocity();
        body->applyImpulse(body->getMass() * vel * .7); // Double the current velocity!
        this->_currentState = Character::State::MID_AIR;
        body->setRotationEnable(false);
        body->setAngularVelocity(0.0);
        this->setRotation(0.0);
        this->setAnimation(0, "JumpForwardFromSwing", false);
        return;
    }
    if (_state == NORMAL) {
        
    }
    Character::jump(jumpPower);
}

void Monkey::characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) {
    if (code == cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW) {
        if (_state == CLIMBING) {
            if (pressed) {
                climbUpVel += 200;
            } else {
                climbUpVel -= 200;
            }
            updateClimbingVel();
        } else if (_state == SWINGING) {
            //pinJoint->removeFormWorld();
            
        }
    }
    
    if (code == cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
        if (_state == CLIMBING) {
            if (pressed) {
                climbDownVel += 200;
            } else {
                climbDownVel -= 200;
            }
            updateClimbingVel();
        } else if (_state == SWINGING) {
        
        }
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
    if (pinJoint != nullptr) {
        pinJoint->removeFormWorld();
        pinJoint = nullptr;
    }
    if (gearJoint != nullptr) {
        gearJoint->removeFormWorld();
        gearJoint = nullptr;
    }
    // create a joint between you and the vine.
    pinJoint = cocos2d::PhysicsJointPin::construct(this->body, vine, cocos2d::Vec2::ZERO, cocos2d::Vec2(0, offset));
    world->addJoint(pinJoint);
    _state = SWINGING;
    this->setTimeScale(2.0);
    this->setAnimation(0, "JumpToSwing", false);
    this->addAnimation(0, "Swing", true);
    //this->setTimeScale(1.0);
    // set the character to rotate with the vine.
    body->setRotationEnable(true);
    body->setRotationOffset(0.0);
    double gearPhase = 0.0;
    double gearRatio = 1.0;
    gearJoint = cocos2d::PhysicsJointGear::construct(body, vine, gearPhase, gearRatio);
    world->addJoint(gearJoint);
}
