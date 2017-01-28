#include "Character.h"
#include <iostream>
#include <array>

using namespace JTTW;

Character::Character(const std::string artFilePrefix, JTTW::Rectangle dimensions, cocos2d::Vec2 maxVelocities, double gravity) :
        ani(spine::SkeletonAnimation::createWithJsonFile(artFilePrefix + ".json", artFilePrefix + ".atlas", 1.0f)),
        characterName(artFilePrefix),
        dimensions(dimensions),
        _maxVelocities(maxVelocities),
        _gravity(gravity),
        collisionBoxNode(cocos2d::DrawNode::create()) {
    
    ani->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    updatePosition(dimensions.getCenterX(), dimensions.getCenterY());
    ani->setScaleX(dimensions.getWidth() / 720.0f); // 720.0px is approximately the size of the art at 1.0f.
    ani->setScaleY(dimensions.getHeight() / 720.0f);
            
    ani->setAnimation(0, "idle", true);
}

Character::~Character() {}

void Character::accelerateLeft(float deltaVel) {
    forceXLeft += deltaVel;
    velocities.x = forceXRight - forceXLeft;
    updateAnimation();
}

void Character::accelerateRight(float deltaVel) {
    forceXRight += deltaVel;
    velocities.x = forceXRight - forceXLeft;
    updateAnimation();
}

void Character::stop() {
    velocities.x = 0.0;
    forceXRight = 0.0;
    forceXLeft = 0.0;
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

void Character::move(float deltaTime, std::vector<Platform> platforms, bool debugOn) {
    auto position = dimensions.getCenter();
    position.x += velocities.x * _maxVelocities.x * deltaTime;
    position.y += velocities.y * _maxVelocities.y * deltaTime;
    if (currentState == State::MID_AIR) {
        velocities.y -= (_gravity * deltaTime) / _maxVelocities.y;
    }
    bool collision = false;
    Platform collidedPlat;
    JTTW::Rectangle me(position.x, position.y, dimensions.getWidth(), dimensions.getHeight());
    for (auto plat = platforms.begin(); plat != platforms.end(); plat++) {
        if (me.isInCollisionWith(plat->getCollisionBounds())) {
            collision = true;
            collidedPlat = *plat;
            break;
        }
    }

    if (collision == true) {
        // push to out of box
        position = me.closestNonCollidingPoint(collidedPlat.getCollisionBounds());
        JTTW::Rectangle r = collidedPlat.getCollisionBounds();
        if (me.getMaxX() > r.getMinX() && me.getMinX() < r.getMaxX()) {
            velocities.y = 0.0;
            if (me.getCenterY() > r.getMaxY() && currentState != State::STANDING) {
                currentState = State::STANDING;
                updateAnimation();
            }
        }
        if (me.getCenterY() + me.getHeight() / 2.0 > r.getMinY() && me.getCenterY() - me.getHeight() / 2.0 < r.getMaxY()) {
            velocities.x = 0.0;
        }
        
    } else if (collision == false && position.y > 0.0) {
        // check vertical column
        bool isHovering = false;
        for (auto plat = platforms.begin(); plat != platforms.end(); plat++) {
            if (me.isDirectlyAbove((*plat).getCollisionBounds())) {
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

void Character::drawCollisionBox() {
    cocos2d::Color4F black(0.0, 1.0, 0.0, 1.0);
    collisionBoxNode->clear();
    collisionBoxNode->drawPolygon(dimensions.getPoints().data(), 4, black, 0, black);
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
