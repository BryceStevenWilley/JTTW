//
//  PCharacter.cpp
//  JTTW
//
//  Created by Bryce Willey on 2/3/17.
//
//

#include "Character.hpp"
#include "Collisions.hpp"
#include "Monkey.hpp"
#include "Monk.hpp"
#include "Piggy.hpp"
#include "Sandy.hpp"
#include <iostream>

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
        body(cocos2d::PhysicsBody::createBox(cocos2d::Size(600.0f, 720.0f), mat)),
        characterName(artFilePrefix) {
    
    this->initWithJsonFile(artFilePrefix + ".json", artFilePrefix + ".atlas", 1.0f);
    this->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    this->setScaleX(dimensions.width / 720.0f); // 720.0px is approximately the size of the art at 1.0f.
    this->setScaleY(dimensions.height / 720.0f);

    body->setCategoryBitmask((int)CollisionCategory::Character);
    body->setCollisionBitmask((int)CollisionCategory::PlatformAndBoulder);
    body->setContactTestBitmask((int)CollisionCategory::PlatformAndBoulder);
    body->setRotationEnable(false);

    body->setPositionOffset(cocos2d::Vec2(0.0, dimensions.height / 2.0));

    body->setVelocityLimit(600);

    this->setPhysicsBody(body);
    this->setAnimation(0, "idle", true);

    followcrown = cocos2d::Sprite::create("Selection Crown.png");
    followcrown->setScale(.2);
    followcrown->setPosition(0.0, 950);
    followcrown->setVisible(false);
    this->addChild(followcrown);
    
    alonecrown = cocos2d::Sprite::create("AloneCrown.png");
    alonecrown->setScale(.2);
    alonecrown->setPosition(0.0, 940);
    alonecrown->setVisible(false);
    this->addChild(alonecrown);

    currentCrown = alonecrown;

    this->setPosition(startPosition);
    
    _startingPosition = startPosition;
    
    this->setTag(CHARACTER_TAG);
}

void Character::impulseLeft(float deltaVel) {
    double impulse = body->getMass() * deltaVel;
    leftMomentum += impulse;
    rebalanceImpulse();
}

void Character::impulseRight(float deltaVel) {
    double impulse = body->getMass() * deltaVel;
    rightMomentum += impulse;
    rebalanceImpulse();
}

void Character::applyForceRight(double fprime_x) {
    cocos2d::Vec2 oldVel = cocos2d::Vec2(body->getVelocity().x, body->getVelocity().y);
    cocos2d::Vec2 F_x = cocos2d::Vec2(fprime_x * body->getMass(), 0);
    body->applyForce(F_x);
}

void Character::rebalanceImpulse() {
    std::cout << characterName << " right momentum: " << rightMomentum << ", left: " << leftMomentum << std::endl;
    double totalMomentum = rightMomentum - leftMomentum;
    double targetVelocity = totalMomentum / body->getMass();
    
    double actualDeltaVel = targetVelocity - body->getVelocity().x;
    
    double actualImpulse = body->getMass() * actualDeltaVel;
    
    body->applyImpulse(cocos2d::Vec2(actualImpulse, 0));
}

void Character::stop() {
    leftMomentum = 0.0;
    rightMomentum = 0.0;
    body->applyImpulse(cocos2d::Vec2(body->getMass() * -(body->getVelocity().x), 0));
}
    
void Character::jump(double force) {
    if (_currentState == State::MID_AIR) {
        // Can't jump while you're in the air, dummy!
        return;
    }
    
    body->applyImpulse(cocos2d::Vec2(0, body->getMass() * force));
}

void Character::jumpFromForce(double fprime_y) {
    if (_currentState == State::MID_AIR) {
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

void Character::landedCallback() {
    State oldState = _currentState;
    _currentState = State::STANDING;
    platformsStandingOn += 1;
    updateAnimation(oldState);
}

void Character::leftCallback() {
    platformsStandingOn -= 1;
    if (platformsStandingOn == 0) {
        State oldState = _currentState;
        _currentState = State::MID_AIR;
        updateAnimation(oldState);
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

void Character::updateAnimation() {
    if (_oldVel == body->getVelocity()) {
        // nothing changed.
        return;
    }
    if (std::abs(_oldVel.x) >= 5 && std::abs(body->getVelocity().x) < 5) {
        // Slowing down.
        this->setAnimation(0, "idle", true);
    }
    if (std::abs(_oldVel.x) < 5 && std::abs(body->getVelocity().x) >= 5) {
        // Speeding up.
        this->setAnimation(0, "walk", true);
    }
    
    //  Left vs. Right
    if (_oldVel.x <= 0.0 && body->getVelocity().x > 0.0) {
      // Set to go right.
      this->setScaleX(std::abs(this->getScaleX()));
    }
    if (_oldVel.x >= 0.0 && body->getVelocity().x < 0.0) {
      this->setScaleX(-1 * std::abs(this->getScaleX()));
    }
    
    _oldVel = body->getVelocity();
}

void Character::updateAnimation(State oldState) {
   if (oldState == State::MID_AIR && _currentState == State::STANDING) {
        this->setTimeScale(1.0);
        if (body->getVelocity().x > 10.0 || body->getVelocity().x < -10.0) {
            // TODO: Set walk or run depending on the speed (interpolate?)
            this->setAnimation(0, "walk", true);
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
}

void Character::restartFromStart() {
    body->setVelocity(cocos2d::Vec2(0, 0));
    this->setPosition(_startingPosition);
}

