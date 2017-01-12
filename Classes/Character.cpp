#include "Character.h"
#include <stdio.h>

using namespace JTTW;

Character::Character(const std::string artFileName, cocos2d::Vec2 dimensions) : sprite(cocos2d::Sprite::create(artFileName)), dimensions(dimensions) {
    
    sprite->setAnchorPoint(cocos2d::Vec2(0.5, 0.0));

    
    cocos2d::Size imgDim = sprite->getTextureRect().size;
    float xScale = dimensions.x / imgDim.width;
    float yScale = dimensions.y / imgDim.height;
    sprite->setScaleX(xScale);
    sprite->setScaleY(yScale);
}

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
