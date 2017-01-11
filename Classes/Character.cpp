
#include "Character.h"
#include <stdio.h>

using namespace JTTW;

Character::Character(const std::string artFileName) : sprite(cocos2d::Sprite::create(artFileName)) {}

Character::~Character() {}

void Character::move(float deltaTime) {
    auto position = sprite->getPosition();
    position.x += velocities.x * maxVelocities.x * deltaTime;
    position.y += velocities.y * maxVelocities.y * deltaTime;
    if (currentState == State::MID_AIR) {
        velocities.y -= 5.0 * deltaTime; // TODO: figure out units, and stop dealing with pixels.
    }
    if (position.y <= 0.0) {
        position.y = 0.0;
        currentState = State::STANDING;
        velocities.y = 0.0;
    }
    sprite->setPosition(position);
}
