#include "Character.h"
#include <iostream>

using namespace JTTW;

Character::Character(const std::string artFilePrefix, cocos2d::Vec2 dimensions, cocos2d::Vec2 maxVelocities, double gravity) :
        ani(spine::SkeletonAnimation::createWithJsonFile(artFilePrefix + ".json", artFilePrefix + ".atlas", 1.0f)),
        //cocos2d::Sprite::create(artFileName)),
        dimensions(dimensions),
        _maxVelocities(maxVelocities),
        _gravity(gravity) {
            
    ani->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
    ani->setScaleX(dimensions.x / 720.0f); // 720.0px is approximately the size of the art at 1.0f.
    ani->setScaleY(dimensions.y / 720.0f);
            
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

bool isDirectlyAbove(cocos2d::Rect platform, cocos2d::Vec2 center, cocos2d::Vec2 dims) {
    float maxX = center.x + dims.x / 2.0;
    float minX = center.x - dims.x / 2.0;
    if (maxX > platform.getMinX() && minX < platform.getMaxX()) {
        float minY = center.y - dims.y / 2.0;
        if (minY >= platform.getMaxY() && minY < platform.getMaxY() + 10) {
            return true;
        }
    }
    return false;
}

bool isInCollision(cocos2d::Rect platform, cocos2d::Vec2 center, cocos2d::Vec2 dims) {
    float maxX = center.x + dims.x / 2.0;
    float minX = center.x - dims.x / 2.0;
    if (maxX > platform.getMinX() && minX < platform.getMaxX()) {
        float maxY = center.y + dims.y / 2.0;
        float minY = center.y - dims.y / 2.0;
        if (maxY > platform.getMinY() && minY < platform.getMaxY()) {
            return true;
        }
    }
    return false;
}

cocos2d::Vec2 collidedPosition(cocos2d::Rect platform, cocos2d::Vec2 center, cocos2d::Vec2 dims, cocos2d::Vec2 velos) {
    float maxX = center.x + dims.x / 2.0;
    float minX = center.x - dims.x / 2.0;
    float maxY = center.y + dims.y / 2.0;
    float minY = center.y - dims.y / 2.0;
    float bottomDiff = maxY - platform.getMinY();
    float topDiff = platform.getMaxY() - minY;
    float leftDiff = maxX - platform.getMinX();
    float rightDiff = platform.getMaxX() - minX;
    
    //std::cout << "Stats: MaxX: " << maxX << ", MinX: " << minX << ", MaxY: " << maxY << ", MinY: " << minY << std::endl;
    //std::cout << "       Bott: " << bottomDiff << ", Top:  " << topDiff << ", left: " << leftDiff << ", righ: " << rightDiff << std::endl;
    
    
    // TODO: WOW THIS IS SHIT.
    if (topDiff < bottomDiff) {
        // collision on top.
        if (leftDiff < rightDiff) {
            // top left
            if (topDiff < leftDiff) {
                // top.
                //std::cout << "Top collinion" << std::endl;
                return cocos2d::Vec2(center.x, platform.getMaxY() + dims.y / 2.0);
            } else {
                // left
                //std::cout << "Left collinion" << std::endl;
                return cocos2d::Vec2(platform.getMinX() - dims.x / 2.0, center.y);
            }
        } else {
            // top right
            if (topDiff < rightDiff) {
                //std::cout << "Top collinion" << std::endl;
                return cocos2d::Vec2(center.x, platform.getMaxY() + dims.y / 2.0);
            } else {
                //std::cout << "Right collinion" << std::endl;
                return cocos2d::Vec2(platform.getMaxX() + dims.x / 2.0, center.y);
            }
        }
    } else {
        // collision on bottom.
        if (leftDiff < rightDiff) {
            // bottom left
            if (bottomDiff < leftDiff) {
                //std::cout << "Bottom collision" << std::endl;
                return cocos2d::Vec2(center.x, platform.getMinY() - dims.y / 2.0);
            } else {
                //std::cout << "Left collision" << std::endl;
                return cocos2d::Vec2(platform.getMinX() - dims.x / 2.0, center.y);
            }
        } else {
            if (bottomDiff < rightDiff) {
                //std::cout << "Bottom collision" << std::endl;
                return cocos2d::Vec2(center.x, platform.getMinY() - dims.y / 2.0);
            } else {
                //std::cout << "Right collision" << std::endl;
                return cocos2d::Vec2(platform.getMaxX() + dims.x / 2.0, center.y);
            }
        }
    }
}

void Character::move(float deltaTime, std::vector<BadPlatform> platforms) {
    auto position = ani->getPosition();
    position.x += velocities.x * _maxVelocities.x * deltaTime;
    position.y += velocities.y * _maxVelocities.y * deltaTime;
    if (currentState == State::MID_AIR) {
        velocities.y -= (_gravity * deltaTime) / _maxVelocities.y;
    }
    bool collision = false;
    BadPlatform collidedPlat;
    for (auto plat = platforms.begin(); plat != platforms.end(); plat++) {
        if (isInCollision((*plat).dimensions, position, dimensions)) {
            collision = true;
            collidedPlat = *plat;
            break;
        }
    }
    
    if (position.y < 0.0) {
        position.y = 0.0;
        currentState = State::STANDING;
        velocities.y = 0.0;
        updateAnimation();
    } else if (collision == true) {
        // push to out of box
        position = collidedPosition(collidedPlat.dimensions, position, dimensions, velocities);
        cocos2d::Rect r = collidedPlat.dimensions;
        if (position.x + dimensions.x / 2.0 > r.getMinX() && position.x - dimensions.x / 2.0 < r.getMaxX()) {
            velocities.y = 0.0;
            if (position.y > r.getMaxY() && currentState != State::STANDING) {
                std::cout << "current State: " << currentState << ", standing == " << State::STANDING << std::endl;
                currentState = State::STANDING;
                std::cout << "Calling from collision" << std::endl;
                updateAnimation();
            }
        }
        if (position.y + dimensions.y / 2.0 > r.getMinY() && position.y - dimensions.y / 2.0 < r.getMaxY()) {
            velocities.x = 0.0;
        }
        
    } else if (collision == false && position.y > 0.0) {
        // check vertical column
        bool isHovering = false;
        for (auto plat = platforms.begin(); plat != platforms.end(); plat++) {
            if (isDirectlyAbove((*plat).dimensions, position, dimensions)) {
                isHovering = true;
                break;
            }
        }
        if (!isHovering) {
            std::cout << "Setting state to mid air" << std::endl;
            currentState = State::MID_AIR;
            velocities.x = forceXRight - forceXLeft;
        }
    }
    ani->setPosition(position);
}

void Character::updateAnimation() {
    std::cout << "Updating animation " << rand() << std::endl;
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
        ani->setTimeScale(0.5);
    }
}
