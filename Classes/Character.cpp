#include "Character.h"
#include <stdio.h>

using namespace JTTW;

Character::Character(const std::string artFilePrefix, cocos2d::Vec2 dimensions, cocos2d::Vec2 maxVelocities, double gravity) :
        ani(spine::SkeletonAnimation::createWithJsonFile(artFilePrefix + ".json", artFilePrefix + ".atlas", 1.0f)),
        //cocos2d::Sprite::create(artFileName)),
        dimensions(dimensions),
        _maxVelocities(maxVelocities),
        _gravity(gravity) {
            
    ani->setAnchorPoint(cocos2d::Vec2(0.5, 0.0));
    ani->setScaleX(dimensions.x / 720.0f); // 720.0px is approximately the size of the art at 1.0f.
    ani->setScaleY(dimensions.y / 720.0f);
            
    ani->setAnimation(0, "idle", true);
}

Character::~Character() {}

void Character::accelerateLeft(float deltaVel) {
    velocities.x -= deltaVel;
    updateAnimation();
}

void Character::accelerateRight(float deltaVel) {
    velocities.x += deltaVel;
    updateAnimation();
}

void Character::stop() {
    velocities.x = 0.0;
    updateAnimation();
}

void Character::jump(float percent) {
    velocities.y = percent;
    currentState = State::MID_AIR;
    updateAnimation();
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

void Character::move(float deltaTime, std::vector<cocos2d::Sprite *> platforms) {
    auto position = ani->getPosition();
    position.x += velocities.x * _maxVelocities.x * deltaTime;
    position.y += velocities.y * _maxVelocities.y * deltaTime;
    if (currentState == State::MID_AIR) {
        velocities.y -= (_gravity * deltaTime) / _maxVelocities.y;
    }
    bool vertCollision = false;
    cocos2d::Sprite *collidedPlat;
    for (auto plat = platforms.begin(); plat != platforms.end(); plat++) {
        if(position.x  >= (*plat)->getBoundingBox().getMinX() && position.x <= (*plat)->getBoundingBox().getMaxX()) {
            if (position.y >= (*plat)->getBoundingBox().getMinY() && position.y <= (*plat)->getBoundingBox().getMaxY()) {
                vertCollision = true;
                collidedPlat = *plat;
                break;
            }
        }
    }
    
    if (position.y < 0.0) {
        position.y = 0.0;
        currentState = State::STANDING;
        velocities.y = 0.0;
        updateAnimation();
    } else if (vertCollision == true) {
        position.y = collidedPlat->getBoundingBox().getMaxY();
        currentState = State::STANDING;
        velocities.y = 0.0;
        updateAnimation();
    } else if (vertCollision == false) {
        currentState = State::MID_AIR;
    }
    ani->setPosition(position);
}

void Character::updateAnimation() {
    if (currentState == STANDING) {
        ani->setTimeScale(1.0);
        if (velocities.x > 0.0) {
            // If moving right, make the x scale positive so animation is facing right.
            ani->setScaleX(std::abs(ani->getScaleX()));
            ani->setAnimation(0, "run", true);
        } else if (velocities.x < 0.0) {
            // If moving left, make the x scale negative so the animation is facing left.
            ani->setScaleX(-1 * std::abs(ani->getScaleX()));
            ani->setAnimation(0, "run", true);
        } else { // x == 0.0
            ani->setAnimation(0, "idle", true);
        }
    } else if (currentState == MID_AIR && velocities.y > .9) {
        // If the character is in mid air and traveling upwards (usually happens right after jumping)
        // then set the jump animation and slow it down so it lasts the whole time you're in the air.
        ani->setAnimation(0, "jump", false);
        ani->setTimeScale(0.5);
    }
}
