#include "Monkey.hpp"
#include "SimpleAudioEngine.h"
#include "Resolution.hpp"
#include "Platform.hpp"
#include "Collisions.hpp"

using namespace JTTW;

const double Monkey::JUMP_INIT = ideal2Res(600);
const double Monkey::CLIMB_VEL = ideal2Res(200);
const double Monkey::VINE_CLIMB_INC = ideal2Res(30);
const cocos2d::PhysicsMaterial MONKEY_MATERIAL = cocos2d::PhysicsMaterial(0.8, 0.0, 1.0);

Monkey::Monkey(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Monkey", MONKEY_MATERIAL, startPosition, dimensions), _state(NORMAL) {}

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
        body->applyImpulse(body->getMass() * vel * .85); // Double the current velocity!
        if (vel.x < 0) {
            body->applyImpulse(body->getMass() * cocos2d::Vec2(ideal2Res(-100), ideal2Res(200)));
        } else {
            body->applyImpulse(body->getMass() * cocos2d::Vec2(ideal2Res(100), ideal2Res(200)));
        }
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
            if (pressed) {
                moveAlongClimbable(VINE_CLIMB_INC * _upDir);
            }
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
            if (pressed) {
                moveAlongClimbable(-VINE_CLIMB_INC * _upDir);
            }
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
}

void Monkey::enteringClimeable(cocos2d::PhysicsWorld *world, SceneObject *p, cocos2d::Vec2 offset, cocos2d::Vec2 upDir, bool alreadyOn) {//, Platform *platform) {
    leavingClimeable();
    pinJoint = cocos2d::PhysicsJointPin::construct(this->body, p->getPhysicsBody(), cocos2d::Vec2::ZERO, offset);
    world->addJoint(pinJoint);
    currentAttached = p;
    currentAttachedOffset = offset;
    
    if (!alreadyOn) {
        this->setTimeScale(2.0);
        this->setAnimation(0, "ClimbIdle", true);
    } else {
        this->setTimeScale(2.0);
        //this->setTimeScale(1.0);
        this->setAnimation(0, "Climb", true);
        this->addAnimation(0, "ClimbIdle", true);
    }
    
    if (_state == NORMAL) {
        std::cout << "Now climbing" << std::endl;
        // Stick to where you are.
        //body->setGravityEnable(false);
        //Character::stop();
        //body->setVelocity(cocos2d::Vec2::ZERO);
        _state = CLIMBING;
        if (upDir.y < 0) {
            _upDir = -upDir;
        } else {
            _upDir = upDir;
        }
        //this->setAnimation(0, "ClimbIdle", true);
        //updateClimbingVel(); // Because we're still tracking keypresses even off of the trees.
    }
    currentWorld = world;
}

void Monkey::moveAlongClimbable(cocos2d::Vec2 up) {
    std::cout << "Current Attached offset: " << currentAttachedOffset.x << ", " << currentAttachedOffset.y << std::endl;
    std::cout << "Current Attached range: " << currentAttached->getYRange() << std::endl;
    std::cout << "Up: " << up.x << ", " << up.y << std::endl;
    if ((currentAttachedOffset + up).y > -currentAttached->getYRange() / 2.0 &&
        (currentAttachedOffset + up).y < currentAttached->getYRange() / 2.0) {
        enteringClimeable(currentWorld, currentAttached, currentAttachedOffset + up, _upDir, true);
    }
}

void Monkey::leavingClimeable() {
    std::cout << "Leaving climbing" << std::endl;
    if (pinJoint != nullptr) {
        pinJoint->removeFormWorld();
        pinJoint = nullptr;
    }
    
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
void Monkey::enteringVine(cocos2d::PhysicsWorld *world, SceneObject *obj, double offset, bool alreadyOn) {
    leavingVine();
    // create a joint between you and the vine.
    cocos2d::Vec2 offsetVec = cocos2d::Vec2(0, ideal2Res(offset));
    pinJoint = cocos2d::PhysicsJointPin::construct(this->body, obj->getPhysicsBody(), cocos2d::Vec2::ZERO, offsetVec);
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
    gearJoint = cocos2d::PhysicsJointGear::construct(body, obj->getPhysicsBody(), gearPhase, gearRatio);
    world->addJoint(gearJoint);

    // set these params so you can climb later.
    currentAttached = obj;
    currentAttachedOffset = offsetVec;
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

    currentAttached = nullptr;
    currentAttachedOffset = cocos2d::Vec2::ZERO;
    
    _state = NORMAL;
    this->_currentState = Character::State::MID_AIR;
    body->setRotationEnable(false);
    body->setAngularVelocity(0.0);
    this->setRotation(0.0);
}

void Monkey::moveAlongVine(float deltaP) {
    if ((currentAttachedOffset + cocos2d::Vec2(0, deltaP)).y > -currentAttached->getYRange() / 2.0 &&
        (currentAttachedOffset + cocos2d::Vec2(0, deltaP)).y < currentAttached->getYRange() / 2.0) {
        enteringVine(currentWorld, currentAttached, (currentAttachedOffset + cocos2d::Vec2(0, deltaP)).y, true);
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

  
void Monkey::setBoulderBury() {
    body->setDynamic(false);
    body->setGravityEnable(false);
    body->setRotationEnable(false);
    freeze();
    findSlot("Body")->a = 0.0;
    findSlot("L Arm")->a = 0.0;
    findSlot("L Calf")->a = 0.0;
    findSlot("L Foot")->a = 0.0;
    findSlot("L Thigh")->a = 0.0;
    findSlot("R Calf")->a = 0.0;
    findSlot("R Foot")->a = 0.0;
    findSlot("R Thigh")->a = 0.0;
    findSlot("R Arm")->a = 0.0;
}

void Monkey::setBoulderUnbury() {
    body->setDynamic(true);
    body->setGravityEnable(true);
    body->setRotationEnable(false);
    setRotation(0.0);
    findSlot("Body")->a = 255.0;
    findSlot("L Arm")->a = 255.0;
    findSlot("L Calf")->a = 255.0;
    findSlot("L Foot")->a = 255.0;
    findSlot("L Thigh")->a = 255.0;
    findSlot("R Calf")->a = 255.0;
    findSlot("R Foot")->a = 255.0;
    findSlot("R Thigh")->a = 255.0;
    findSlot("R Arm")->a = 255.0;
    update(0);
    auto newBody = cocos2d::PhysicsBody::create();
    double width = 480.0f;
    double height = 780.0f;
    auto bodyShape = cocos2d::PhysicsShapeBox::create(cocos2d::Size(width * .95, height * (2.0/3.0)), MONKEY_MATERIAL, cocos2d::Vec2(0.0, height/9.0));
    auto bottomSemiCircle = cocos2d::PhysicsShapeCircle::create(width/2.0, MONKEY_MATERIAL, cocos2d::Vec2(0.0, -height * (0.8/4.0)));
    newBody->addShape(bodyShape);
    newBody->addShape(bottomSemiCircle);
    newBody->setCategoryBitmask((int)CollisionCategory::Character);
    newBody->setCollisionBitmask((int)CollisionCategory::PlatformAndBoulder);
    newBody->setContactTestBitmask((int)CollisionCategory::PlatformBoulderAndProjectile);
    newBody->setRotationEnable(false);
    newBody->setVelocityLimit(600);
    
    removeComponent(body);
    body->removeFromWorld();
    
    addComponent(newBody);
    
    body = newBody;

    setSlotsToSetupPose();
}
