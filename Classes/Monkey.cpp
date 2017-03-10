#include "Monkey.hpp"
#include "SimpleAudioEngine.h"

using namespace JTTW;

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

void Monkey::jump() {
    float jumpPower = 500;
    if (_state == SWINGING) {
        leavingVine();
        
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Sound/Walking.wav");
        this->_currentState = Character::State::STANDING;
        jumpPower = 90;
        _state = NORMAL;
        //Character::jump(jumpPower);
        cocos2d::Vec2 vel = body->getVelocity();
        body->applyImpulse(body->getMass() * vel * .7); // Double the current velocity!
        this->_currentState = Character::State::MID_AIR;
        this->setAnimation(0, "JumpForwardFromSwing", false);
        return;
    }
    if (_state == NORMAL) {
        Character::jump(jumpPower);
    }
    if (_state == CLIMBING) {
        std::cout << "Currently Climbing" << std::endl;
        leavingClimeable();
        this->_currentState = Character::State::STANDING;
        jumpPower = 400;
        Character::jump(jumpPower);
        this->_currentState = Character::State::MID_AIR;
    }
}

void Monkey::characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) {
    if (code == cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW) {
        if (pressed) {
            climbUpVel += 200;
        } else {
            climbUpVel -= 200;
        }
        if (_state == CLIMBING) {
            updateClimbingVel();
        } else if (_state == SWINGING) {
            if (pressed) {
                moveAlongVine(30);
            }
        }
    }
    
    if (code == cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
        if (pressed) {
            climbDownVel += 200;
        } else {
            climbDownVel -= 200;
        }
        if (_state == CLIMBING) {
            updateClimbingVel();
        } else if (_state == SWINGING) {
            moveAlongVine(-30);
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
        std::cout << "Now climbing" << std::endl;
        // Stick to where you are.
        body->setGravityEnable(false);
        //Character::stop();
        body->setVelocity(cocos2d::Vec2::ZERO);
        _state = CLIMBING;
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

void Monkey::enteringVine(cocos2d::PhysicsWorld *world, Vine *vine, double offset, cocos2d::Vec2 collisionPoint) {
    leavingVine();
    // create a joint between you and the vine.
    pinJoint = cocos2d::PhysicsJointPin::construct(this->body, vine->getPhysicsBody(), cocos2d::Vec2::ZERO, cocos2d::Vec2(0, offset));
    world->addJoint(pinJoint);
    _state = SWINGING;
    this->setTimeScale(2.0);
    this->setAnimation(0, "JumpToSwing", false);
    this->addAnimation(0, "Swing", true);
    
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
    std::cout << "Moving along vine!: " << deltaP << " much, currentVineOffset = " << currentVineOffset << std::endl;
    if (currentVineOffset + deltaP > -currentVine->getLength() / 2.0 && currentVineOffset + deltaP < currentVine->getLength() / 2.0) {
        enteringVine(currentWorld, currentVine, currentVineOffset + deltaP, cocos2d::Vec2::ZERO);
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
