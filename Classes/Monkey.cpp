#include "Monkey.hpp"
#include "SimpleAudioEngine.h"
#include "Resolution.hpp"

using namespace JTTW;

const double Monkey::JUMP_INIT = ideal2Res(600);
const double Monkey::CLIMB_VEL = ideal2Res(200);
const double Monkey::VINE_CLIMB_INC = ideal2Res(30);

Monkey::Monkey(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Monkey", cocos2d::PhysicsMaterial(.8, 0.0, 1.0), startPosition, dimensions), _state(NORMAL) {}

void Monkey::impulseLeft(float deltaVel) {
    if (_state == NORMAL || _state == SWINGING) {
        Character::impulseLeft(deltaVel);
    } else { // _state == CLIMBING
        if (this->getScaleX() > 0 && deltaVel > 0) { // Facing right
            leavingClimeable();
            Character::impulseLeft(deltaVel);
        } else {
            Character::impulseLeftButNoRebalance(deltaVel);
        }
    }
}

void Monkey::impulseRight(float deltaVel) {
    if (_state == NORMAL || _state == SWINGING) {
        Character::impulseRight(deltaVel);
    } else { // _state == CLIMBING
        if (this->getScaleX() < 0 && deltaVel > 0) { // Facing right
            leavingClimeable();
            Character::impulseRight(deltaVel);
        } else {
            Character::impulseRightButNoRebalance(deltaVel);
        }
    }
}

void Monkey::initJump() {
    float jumpPower = JUMP_INIT;
    if (_state == SWINGING) {
        leavingVine();

        this->_currentState = Character::State::STANDING;
        _state = NORMAL;
        cocos2d::Vec2 vel = body->getVelocity();
        body->applyImpulse(body->getMass() * vel * .7); // Double the current velocity!
        this->_currentState = Character::State::MID_AIR;
        this->setAnimation(0, "JumpForwardFromSwing", false);
        return;
    }
    if (_state == NORMAL) {
        Character::initJump(jumpPower);
    }
    if (_state == CLIMBING) {
        std::cout << "Currently Climbing" << std::endl;
        leavingClimeable();
        this->_currentState = Character::State::STANDING;
        jumpPower = jumpPower - ideal2Res(100);
        Character::initJump(jumpPower);
        this->_currentState = Character::State::MID_AIR;
    }
}

void Monkey::characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) {
    if (code == cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW) {
        if (pressed) {
            climbUpVel += CLIMB_VEL;
        } else {
            climbUpVel -= CLIMB_VEL;
        }
        if (_state == CLIMBING) {
            updateClimbingVel();
        } else if (_state == SWINGING) {
            if (pressed) {
                moveAlongVine(VINE_CLIMB_INC);
            }
        }
    }
    
    if (code == cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
        if (pressed) {
            climbDownVel += CLIMB_VEL;
        } else {
            climbDownVel -= CLIMB_VEL;
        }
        if (_state == CLIMBING) {
            updateClimbingVel();
        } else if (_state == SWINGING) {
            if (pressed) {
                moveAlongVine(-VINE_CLIMB_INC);
            }
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
    body->setVelocity(cocos2d::Vec2(0, sum).project(_upDir));
}

void Monkey::enteringClimeable(cocos2d::Vec2 upDir) {
    if (_state == NORMAL) {
        std::cout << "Now climbing" << std::endl;
        // Stick to where you are.
        body->setGravityEnable(false);
        //Character::stop();
        body->setVelocity(cocos2d::Vec2::ZERO);
        _state = CLIMBING;
        _upDir = upDir;
        this->setAnimation(0, "ClimbIdle", true);
        updateClimbingVel(); // Because we're still tracking kepresses even off of the trees.
    }
}

void Monkey::leavingClimeable() {
    std::cout << "Leaving climbing" << std::endl;
    if (_state == CLIMBING) {
        this->setTimeScale(1.0);
        this->setAnimation(0, "JumpForwardFromClimb", false);
    }
    _state = NORMAL;
    body->setGravityEnable(true);
}

void Monkey::continueMotion() {
    if (_currentState != Character::State::FROZEN &&
            _state != SWINGING &&
            _state != CLIMBING &&
            wallsHit == 0 && 
            std::abs(rightMomentum - leftMomentum)/body->getMass() > 0.01) {
        rebalanceImpulse();
    }
}

/**
 * 'alreadyOn' is only to change the animation.
 */
void Monkey::enteringVine(cocos2d::PhysicsWorld *world, Vine *vine, double offset, cocos2d::Vec2 collisionPoint, bool alreadyOn) {
    leavingVine();
    // create a joint between you and the vine.
    pinJoint = cocos2d::PhysicsJointPin::construct(this->body, vine->getPhysicsBody(), cocos2d::Vec2::ZERO, cocos2d::Vec2(0, ideal2Res(offset)));
    world->addJoint(pinJoint);
    _state = SWINGING;
    
    if (!alreadyOn) {
        this->setTimeScale(2.0);
        this->setAnimation(0, "JumpToSwing", false);
        this->addAnimation(0, "Swing", true);
    } else {
        this->setTimeScale(2.0);
        this->setTimeScale(1.0);
        this->setAnimation(0, "Climb", false);
        this->addAnimation(0, "Swing", true);
    }
    
    // set the character to rotate with the vine.
    body->setRotationEnable(true);
    body->setRotationOffset(0.0);
    double gearPhase = 0.0;
    double gearRatio = 1.0;
    gearJoint = cocos2d::PhysicsJointGear::construct(body, vine->getPhysicsBody(), gearPhase, gearRatio);
    world->addJoint(gearJoint);

    // set these params so you can climb later.
    currentVine = vine;
    currentVineOffset = offset;
    currentWorld = world;
}

void Monkey::leavingVine() {
        if (pinJoint != nullptr) {
            pinJoint->removeFormWorld();
            pinJoint = nullptr;
        }
        
        if (gearJoint != nullptr) {
            gearJoint->removeFormWorld();
            gearJoint = nullptr;
        }

        currentVine = nullptr;
        currentVineOffset = 0.0;
    
        _state = NORMAL;
        this->_currentState = Character::State::MID_AIR;
        body->setRotationEnable(false);
        body->setAngularVelocity(0.0);
        this->setRotation(0.0);
}

void Monkey::moveAlongVine(float deltaP) {
    if (currentVineOffset + deltaP > -currentVine->getLength() / 2.0 && currentVineOffset + deltaP < currentVine->getLength() / 2.0) {
        enteringVine(currentWorld, currentVine, currentVineOffset + deltaP, cocos2d::Vec2::ZERO, true);
    }
}

void Monkey::restartFromRespawn() {
    if (_state == SWINGING) {
        leavingVine();
    } else
    leavingClimeable();
    Character::restartFromRespawn();
    leavingClimeable();
}
