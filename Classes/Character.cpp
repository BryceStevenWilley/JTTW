#include "Character.hpp"
#include "Collisions.hpp"
#include "MoveablePlatform.hpp"
#include "Monkey.hpp"
#include "Monk.hpp"
#include "Piggy.hpp"
#include "Sandy.hpp"
#include <iostream>
#include "SimpleAudioEngine.h"

using namespace JTTW;

Character * Character::createFromName(const std::string name, cocos2d::Vec2 startPosition, cocos2d::Size dimensions) {
    if (name == "Monkey") {
        return (Character *)new Monkey(startPosition, dimensions);
    } else if (name == "Monk") {
        return (Character *)new Monk(startPosition, dimensions);
    } else if (name == "Piggy") {
        return (Character *)new Piggy(startPosition, dimensions);
    } else { // (name == "Sandy"), ALSO DEFAULT
        return (Character *)new Sandy(startPosition, dimensions);
    }
}

Character::Character(const std::string artFilePrefix, cocos2d::PhysicsMaterial mat, cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
        spine::SkeletonAnimation(),
        body(cocos2d::PhysicsBody::create()),    ///Box(cocos2d::Size(480.0f, 780.0f), mat)),
        characterName(artFilePrefix), _currentState(MID_AIR), _dimensions(dimensions) {
    
    double width = 480.0f;
    double height = 780.0f;
    auto bodyShape = cocos2d::PhysicsShapeBox::create(cocos2d::Size(width, height * (2.0/3.0)), mat, cocos2d::Vec2(0.0, height/9.0));
    auto bottomSemiCircle = cocos2d::PhysicsShapeCircle::create(width/2.0, mat, cocos2d::Vec2(0.0, -height/4.0));
    body->addShape(bodyShape);
    body->addShape(bottomSemiCircle);
    body->setCategoryBitmask((int)CollisionCategory::Character);
    body->setCollisionBitmask((int)CollisionCategory::PlatformAndBoulder);
    body->setContactTestBitmask((int)CollisionCategory::PlatformBoulderAndProjectile);
    body->setRotationEnable(false);

    body->setVelocityLimit(600);
    
    this->initWithJsonFile("characters/" + artFilePrefix + ".json", "characters/" + artFilePrefix + ".atlas", 1.0f);
    //this->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    this->setContentSize(cocos2d::Size(/*480.0f*/0.0, 780.0f));
    this->setScaleX(dimensions.width / 780.0f);
    this->setScaleY(dimensions.height / 780.0f);
    this->setAnimation(0, "idle", true);
    this->setPhysicsBody(body);

    followcrown = cocos2d::Sprite::create("characters/Selection Crown.png");
    followcrown->setScale(.3);
    followcrown->setPosition(0.0, 940);
    followcrown->setVisible(false);
    this->addChild(followcrown);
    
    alonecrown = cocos2d::Sprite::create("characters/AloneCrown.png");
    alonecrown->setScale(.3);
    alonecrown->setPosition(0.0, 940);
    alonecrown->setVisible(false);
    this->addChild(alonecrown);

    currentCrown = alonecrown;

    this->setPosition(startPosition);
    
    _respawnPosition = startPosition;
    
    this->setTag(CHARACTER_TAG);
}

cocos2d::Size Character::getSize() {
    return _dimensions;
}

void Character::impulseLeft(float deltaVel) {
    double impulse = body->getMass() * deltaVel;
    leftMomentum += impulse;
    if (_currentState != State::FROZEN) {
        rebalanceImpulse();
    }
}

void Character::impulseRight(float deltaVel) {
    double impulse = body->getMass() * deltaVel;
    rightMomentum += impulse;
    if (_currentState != State::FROZEN) {
        rebalanceImpulse();
    }
}

void Character::impulseLeftButNoRebalance(float deltaVel) {
    double impulse = body->getMass() * deltaVel;
    leftMomentum += impulse;
}

void Character::impulseRightButNoRebalance(float deltaVel) {
    double impulse = body->getMass() * deltaVel;
    rightMomentum += impulse;
}

void Character::applyForceRight(double fprime_x) {
    //cocos2d::Vec2 oldVel = cocos2d::Vec2(body->getVelocity().x, body->getVelocity().y);
    cocos2d::Vec2 F_x = cocos2d::Vec2(fprime_x * body->getMass(), 0);
    
    
    
    body->applyForce(F_x.project(_rightVector));
}

void Character::rebalanceImpulse() {
    double totalMomentum = rightMomentum - leftMomentum;
    double targetVelocity = totalMomentum / body->getMass();
    
    targetVelocity += lastRefVel.x;
    
    double actualDeltaVel = targetVelocity - body->getVelocity().x;
    
    double actualImpulse = body->getMass() * actualDeltaVel;
    
    auto finalVec = cocos2d::Vec2(actualImpulse, 0);
    body->applyImpulse(finalVec.project(_rightVector));
}

void Character::continueMotion() {
    if (_currentState != State::FROZEN &&
            platformsStandingOn != 0 &&
            std::abs(rightMomentum - leftMomentum)/body->getMass() > 0.01) {
        rebalanceImpulse();
    }
}


void Character::stop() {
    leftMomentum = 0.0;
    rightMomentum = 0.0;
    auto finalVec = body->getMass() * -(body->getVelocity().project(_rightVector));
    body->applyImpulse(finalVec);
}

void Character::freeze() {
    State oldState = _currentState;
    _currentState = State::FROZEN;
    updateAnimation(oldState);
    _frozenTimer = 3.0; // seconds
    body->setVelocity(cocos2d::Vec2(0.0, 0.0)); // ????? (how to stop on a ramp?) body->getVelocity().y));
}

// TODO: break into initJump and stopJump, one for key press and one for key release.
// While the key is pressed, apply a force (that is decaying) under a jump time
// amount.
void Character::jump(double force) {
    if (_currentState == State::MID_AIR || _currentState == State::FROZEN) {
        // Can't jump while you're in the air, dummy!
        return;
    }
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Sound/Jump.wav");
    body->applyImpulse(cocos2d::Vec2(0, body->getMass() * force));
}

void Character::jumpFromForce(double fprime_y) {
    if (_currentState == State::MID_AIR || _currentState == State::FROZEN) {
        // Can't jump while you're in the air, dummy!
        return;
    }

    body->applyForce(cocos2d::Vec2(0.0, fprime_y * body->getMass()));
}

void Character::transferVelocity(Character *reciever) {
    std::cout << characterName << " velocity x: " << body->getVelocity().x << ", giving to " << reciever->characterName << std::endl;
    double totalMomentum = rightMomentum - leftMomentum;
    double targetVelocity = totalMomentum / body->getMass();
    
    reciever->impulseRight(targetVelocity);
    this->stop();
}
    
bool Character::justJumped() const {
    return body->getVelocity().y > 100;
}

void Character::landedCallback(cocos2d::PhysicsBody *plat, cocos2d::Vec2 newRightDir) {
    platformsStandingOn += 1;
    
    // TODO: check if we don't need to do this for plats > 2
    _rightVector = newRightDir;
    
    if (_currentState != State::FROZEN) {
        State oldState = _currentState;
        _currentState = State::STANDING;
        updateAnimation(oldState);
    }
    
    if (plat->getNode()->getTag() == MOVEABLE_TAG) {
        referenceBody = plat;
        body->setVelocity(body->getVelocity() + referenceBody->getVelocity());
        lastRefVel = referenceBody->getVelocity();
    }
}

void Character::leftCallback(cocos2d::PhysicsBody *plat) {
    platformsStandingOn -= 1;
    if (platformsStandingOn == 0) {
        State oldState = _currentState;
        _currentState = State::MID_AIR;
        _rightVector = cocos2d::Vec2(1, 0);
        updateAnimation(oldState);
        if (plat->getNode()->getTag() == MOVEABLE_TAG) {
            referenceBody = nullptr;
            lastRefVel = cocos2d::Vec2::ZERO;
        }
    } else if (platformsStandingOn < 0) {
        std::cerr << "ERROR: how can " << characterName << " stand on negative platforms?!" << std::endl;
        platformsStandingOn = 0;
    }
}

bool Character::isMovingLeft() const {
    return body->getVelocity().x < 0.0;
}

bool Character::isMovingRight() const {
    return body->getVelocity().x > 0.0;
}

float Character::getMass() const {
    return body->getMass();
}

const Character::State Character::getCurrentState() const {
    return _currentState;
}

void Character::updateLoop(float delta) {
    // Kinda unrelated stuff regarding frozen.
    if (_currentState == State::FROZEN) {
        _frozenTimer -= delta;
        std::cout << "timer is " << _frozenTimer  << std::endl;
        if (_frozenTimer <= 0.0) {
            std::cout << "Un-freezing" << std::endl;
            State oldState = _currentState;
            if (platformsStandingOn > 0) {
                _currentState = State::STANDING;
            } else {
                _currentState = State::MID_AIR;
            }
            updateAnimation(oldState);
        }
    }

    // Update velocity if needed.
    if (referenceBody != nullptr && lastRefVel != referenceBody->getVelocity()) {
        body->setVelocity(cocos2d::Vec2(body->getVelocity().x - lastRefVel.x + referenceBody->getVelocity().x, 0.0));
        lastRefVel = referenceBody->getVelocity();
    }
    
    auto currentRelVel = cocos2d::Vec2((rightMomentum - leftMomentum)/ body->getMass(), body->getVelocity().y - lastRefVel.y);
    
    //auto currentRelVel = body->getVelocity() - lastRefVel;
    if (_oldVel == currentRelVel) {
        // nothing changed.
        return;
    }
    if (_currentState == STANDING) {
        if (std::abs(_oldVel.x) >= 5 && std::abs(currentRelVel.x) < 5) {
            // Slowing down.
            this->setAnimation(0, "idle", true);
        }
        if (std::abs(_oldVel.x) < 5 && std::abs(currentRelVel.x) >= 5) {
            // Speeding up.
            this->setAnimation(0, "walk", true);
        }
    }
    
    //  Left vs. Right
    if (_oldVel.x <= 0.0 && currentRelVel.x > 0.0) {
      // Set to go right.
      this->setScaleX(std::abs(this->getScaleX()));
    }
    if (_oldVel.x >= 0.0 && currentRelVel.x < 0.0) {
      this->setScaleX(-1 * std::abs(this->getScaleX()));
    }
    
    _oldVel = currentRelVel;
}

void Character::updateAnimation(State oldState) {
   if ((oldState == State::MID_AIR || oldState == State::FROZEN) && _currentState == State::STANDING) {
        this->setTimeScale(1.0);
        if (body->getVelocity().x > 10.0 || body->getVelocity().x < -10.0) {
            // TODO: Set walk or run depending on the speed (interpolate?)
            this->setAnimation(0, "walk", true);
            //CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Sound/Walking.wav");
        } else { // x == 0.0
            this->setAnimation(0, "idle", true);
        }
   }
   if (oldState == State::STANDING && _currentState == State::MID_AIR) {
        // If the character is in mid air traveling upwards (right after jumping)
        // then set the jump animation and slow it down so it lasts the whole time.
        this->setAnimation(0, "jump", false);
        this->setTimeScale(0.9);
   }

   if (_currentState == State::FROZEN) {
       // TODO: wait for Mei's new animation.
   }

   // TODO: haven't covered from frozen to mid air. (what it do?)
}

void Character::restartFromRespawn() {
    std::cout << "Restarting " << characterName << " at " << _respawnPosition.x << ", " << _respawnPosition.y << std::endl;
    body->setVelocity(cocos2d::Vec2(0, 0));
    this->setPosition(_respawnPosition);
    this->setPosition(_respawnPosition);
}

void Character::setNewRespawn(cocos2d::Vec2 newRespawn) {
    _respawnPosition = newRespawn;
}

double Character::getRespawnProgress() const {
    return _respawnPosition.x;
}

