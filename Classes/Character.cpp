#include "Character.h"
#include <stdio.h>

using namespace JTTW;

Character::Character(const std::string artFileName, cocos2d::Vec2 dimensions, cocos2d::Vec2 maxVelocities, double gravity) :
        sprite(cocos2d::Sprite::create(artFileName)),
        dimensions(dimensions),
        _maxVelocities(maxVelocities),
        _gravity(gravity) {
    
    sprite->setAnchorPoint(cocos2d::Vec2(0.5, 0.0));
    
    cocos2d::Size imgDim = sprite->getTextureRect().size;
    float xScale = dimensions.x / imgDim.width;
    float yScale = dimensions.y / imgDim.height;
    sprite->setScaleX(xScale);
    sprite->setScaleY(yScale);
}

Character::~Character() {}

void Character::accelerateLeft(float deltaVel) {
    velocities.x -= deltaVel;
}

void Character::accelerateRight(float deltaVel) {
    velocities.x += deltaVel;
}

void Character::stop() {
    velocities.x = 0.0;
}

void Character::jump(float percent) {
    velocities.y = percent;
    currentState = State::MID_AIR;
}

void Character::transferVelocity(Character *reciever) {
    reciever->accelerateRight(velocities.x);
    this->stop();
}

bool Character::isMovingLeft() const {
    return velocities.x < 0.0;
}

bool Character::isMovingRight() const {
    return velocities.x > 0.0;
}

bool Character::justJumped() const {
    return velocities.y == 1.0;
}

double Character::getXVelocity() const {
    return velocities.x * _maxVelocities.x;
}

double Character::getYVelocity() const {
    return velocities.y * _maxVelocities.y;
}

const Character::State Character::getCurrentState() const {
    return currentState;
}

void Character::move(float deltaTime) {
    auto position = sprite->getPosition();
    position.x += velocities.x * _maxVelocities.x * deltaTime;
    position.y += velocities.y * _maxVelocities.y * deltaTime;
    if (currentState == State::MID_AIR) {
        velocities.y -= (_gravity * deltaTime) / _maxVelocities.y; // TODO: figure out units, and stop dealing with pixels.
    }
    if (position.y <= 0.0) {
        position.y = 0.0;
        currentState = State::STANDING;
        velocities.y = 0.0;
    }
    sprite->setPosition(position);
}
