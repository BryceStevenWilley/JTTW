#include "Character.h"
#include <iostream>
#include <array>

using namespace JTTW;

Character::Character(const std::string artFilePrefix, JTTW::Rectangle dimensions, cocos2d::Vec2 maxVelocities, double mass, double gravity) : MoveableObject(dimensions, maxVelocities),
        ani(spine::SkeletonAnimation::createWithJsonFile(artFilePrefix + ".json", artFilePrefix + ".atlas", 1.0f)),
        characterName(artFilePrefix),
        _mass(mass),
        _gravity(gravity) {
    
    ani->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    updatePosition(dimensions.getCenterX(), dimensions.getCenterY());
    ani->setScaleX(dimensions.getWidth() / 720.0f); // 720.0px is approximately the size of the art at 1.0f.
    ani->setScaleY(dimensions.getHeight() / 720.0f);
            
    ani->setAnimation(0, "idle", true);
}

Character::~Character() {}

void Character::accelerateLeft(float deltaVel) {
    MoveableObject::accelerateLeft(deltaVel);
    updateAnimation();
}

void Character::accelerateRight(float deltaVel) {
    MoveableObject::accelerateRight(deltaVel);
    updateAnimation();
}

void Character::stop() {
    MoveableObject::stop();
    updateAnimation();
}


void Character::jump(float percent) {
    velocities.y = percent;
    currentState = State::MID_AIR;
    updateAnimation();
}

void Character::transferVelocity(Character *reciever) {
    reciever->accelerateRight(forceXRight);
    reciever->accelerateLeft(forceXLeft);
    this->stop();
}

bool Character::justJumped() const {
    return velocities.y == 1.0;
}

double Character::getMass() const {
    return _mass;
}

const Character::State Character::getCurrentState() const {
    return currentState;
}

void Character::move(float deltaTime, std::vector<GameObject *> platforms, bool debugOn) {
    auto position = dimensions.getCenter();
    position.x += velocities.x * _maxVelocities.x * deltaTime;
    position.y += velocities.y * _maxVelocities.y * deltaTime;
    if (currentState == State::MID_AIR) {
        velocities.y -= (_gravity * deltaTime) / _maxVelocities.y;
    }
    bool collision = false;
    GameObject *collidedPlat;
    JTTW::Rectangle me(position.x, position.y, dimensions.getWidth(), dimensions.getHeight());
    for (auto plat = platforms.begin(); plat != platforms.end(); plat++) {
        if (me.isInCollisionWith((*plat)->dimensions)) {
            collision = true;
            collidedPlat = *plat;
            break;
        }
    }

    if (collision == true) {
        // push to out of box
        JTTW::Rectangle r = collidedPlat->dimensions;
        position = me.closestNonCollidingPoint(r);
        // If the new position moved up or down, then it was a top or bottom collision, so stop vertical velocity.
        if (position.y != me.getCenterY()) {
            velocities.y = 0.0;
            if (me.getCenterY() > r.getMaxY() && currentState != State::STANDING) {
                currentState = State::STANDING;
                updateAnimation();
            }
        }
        if (me.getMaxY() > r.getMinY() && me.getMinY() < r.getMaxY()) {
            velocities.x = 0.0;
        }
        
    } else if (collision == false && position.y > 0.0) {
        // check vertical column
        bool isHovering = false;
        for (auto plat = platforms.begin(); plat != platforms.end(); plat++) {
            if (me.isDirectlyAbove((*plat)->dimensions)) {
                isHovering = true;
                break;
            }
        }
        velocities.x = forceXRight - forceXLeft;
        if (!isHovering) {
            currentState = State::MID_AIR;
        }
    }
    updatePosition(position.x, position.y);
    if (debugOn) {
        drawCollisionBox();
    }
}

void Character::updateAnimation() {
    //std::cout << "Updating animation " << rand() << std::endl;
    if (currentState == STANDING) {
        ani->setTimeScale(1.0);
        if (velocities.x > 0.0) {
            // If moving right, make the x scale positive so animation is facing right.
            ani->setScaleX(std::abs(ani->getScaleX()));
            ani->setAnimation(0, "walk", true);
        } else if (velocities.x < 0.0) {
            // If moving left, make the x scale negative so the animation is facing left.
            ani->setScaleX(-1 * std::abs(ani->getScaleX()));
            ani->setAnimation(0, "walk", true);
        } else { // x == 0.0
            ani->setAnimation(0, "idle", true);
        }
    } else if (currentState == MID_AIR && velocities.y > .9) {
        // If the character is in mid air and traveling upwards (usually happens right after jumping)
        // then set the jump animation and slow it down so it lasts the whole time you're in the air.
        ani->setAnimation(0, "jump", false);
        ani->setTimeScale(0.9);
    }
}

void Character::updatePosition(double centerX, double centerY) {
    dimensions.setCenter(centerX, centerY);
    ani->setPosition(centerX, centerY - dimensions.getHeight() / 2.0);
}
