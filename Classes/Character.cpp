//
//  PCharacter.cpp
//  JTTW
//
//  Created by Bryce Willey on 2/3/17.
//
//

#include "Character.hpp"
#include "Collisions.hpp"

using namespace JTTW;

Character::Character(const std::string artFilePrefix, cocos2d::PhysicsMaterial mat, cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
        spine::SkeletonAnimation(),
        body(cocos2d::PhysicsBody::createBox(cocos2d::Size(720.0f, 720.0f), mat)),
        characterName(artFilePrefix) {
    
    this->initWithJsonFile(artFilePrefix + ".json", artFilePrefix + ".atlas", 1.0f);
    this->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    this->setScaleX(dimensions.width / 720.0f); // 720.0px is approximately the size of the art at 1.0f.
    this->setScaleY(dimensions.height / 720.0f);

    body->setCategoryBitmask((int)CollisionCategory::Character);
    body->setCollisionBitmask((int)CollisionCategory::Platform);
    body->setRotationEnable(false);

    body->setPositionOffset(cocos2d::Vec2(0.0, dimensions.height / 2.0));

    this->setPhysicsBody(body);
    this->setAnimation(0, "idle", true);

    crown = cocos2d::Sprite::create("Selection Crown.png");
    crown->setScale(.3);
    crown->setPosition(0.0, 950);
    crown->setVisible(false);
    this->addChild(crown);

    this->setPosition(startPosition);
    
    this->setTag(CHARACTER_TAG);
}

void Character::impulseLeft(float deltaVel) {
    body->applyImpulse(cocos2d::Vec2(body->getMass() * -deltaVel, 0));
    updateAnimation();
}

void Character::impulseRight(float deltaVel) {
    body->applyImpulse(cocos2d::Vec2(body->getMass() * deltaVel, 0));
    updateAnimation();
}

void Character::stop() {
    body->applyImpulse(cocos2d::Vec2(body->getMass() * -(body->getVelocity().x), 0));
    updateAnimation();
}
    
void Character::jump() {
    body->applyImpulse(cocos2d::Vec2(0, 5000000));
    _currentState = State::MID_AIR;
    updateAnimation();
}

void Character::transferVelocity(Character *reciever) {
    reciever->impulseRight(body->getVelocity().x);
    this->stop();
}
    
bool Character::justJumped() const {
    return body->getVelocity().y > 100;
}

void Character::landedCallback() {
    _currentState = State::STANDING;
    updateAnimation();
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
    if (body->getVelocity().x > 0.0) {
        //if moving right, make the x scale positive, to face sprite right
        this->setScaleX(std::abs(this->getScaleX()));
    } else if (body->getVelocity().x < 0.0) {
        // If moving left, make the x scale negative to flip the sprite to face left.
        this->setScaleX(-1 * std::abs(this->getScaleX()));
    }
    if (_currentState == STANDING) {
        this->setTimeScale(1.0);
        if (body->getVelocity().x != 0.0) {
            // TODO: Set walk or run depending on the speed (interpolate?)
            this->setAnimation(0, "walk", true);
        } else { // x == 0.0
            this->setAnimation(0, "idle", true);
        }
    } else if (_currentState == MID_AIR && body->getVelocity().y > 100) {
        // If the character is in mid air traveling upwards (right after jumping)
        // then set the jump animation and slow it down so it lasts the whole time.
        this->setAnimation(0, "jump", false);
        this->setTimeScale(0.9);
    }
}
