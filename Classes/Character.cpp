#include "Character.h"
#include <iostream>

using namespace JTTW;

Character::Character(const std::string artFilePrefix, JTTW::Rectangle dimensions, cocos2d::Vec2 maxVelocities, double gravity) :
        ani(spine::SkeletonAnimation::createWithJsonFile(artFilePrefix + ".json", artFilePrefix + ".atlas", 1.0f)),
        //cocos2d::Sprite::create(artFileName)),
        characterName(artFilePrefix),
        dimensions(dimensions),
        _maxVelocities(maxVelocities),
        _gravity(gravity),
        a(cocos2d::DrawNode::create()){
    
    ani->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    ani->setPosition(dimensions.getCenterX(), dimensions.getCenterY());
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

bool Character::isDirectlyAbove(JTTW::Rectangle platform, JTTW::Rectangle me) {
    float maxX = me.getMaxX();
    float minX = me.getMinX();
    if (maxX > platform.getMinX() && minX < platform.getMaxX()) {
        float minY = me.getMinY();
        if (minY >= platform.getMaxY() && minY < platform.getMaxY() + 10) {
            return true;
        }
    }
    return false;
}

bool isInCollision(JTTW::Rectangle platform, JTTW::Rectangle rect) {
    float maxX = rect.getMaxX();
    float minX = rect.getMinX();
    if (maxX > platform.getMinX() && minX < platform.getMaxX()) {
        float maxY = rect.getMaxY();
        float minY = rect.getMinY();
        if (maxY > platform.getMinY() && minY < platform.getMaxY()) {
            return true;
        }
    }
    return false;
}

cocos2d::Vec2 collidedPosition(JTTW::Rectangle platform, JTTW::Rectangle rect, cocos2d::Vec2 velos) {
    float maxX = rect.getMaxX();
    float minX = rect.getMinX();
    float maxY = rect.getMaxY();
    float minY = rect.getMinY();
    float bottomDiff = maxY - platform.getMinY();
    float topDiff = platform.getMaxY() - minY;
    float leftDiff = maxX - platform.getMinX();
    float rightDiff = platform.getMaxX() - minX;

    // TODO: WOW THIS IS SHIT.
    if (topDiff < bottomDiff) {
        // collision on top.
        if (leftDiff < rightDiff) {
            // top left
            if (topDiff < leftDiff) {
                // top.
                //std::cout << "Top collinion" << std::endl;
                return cocos2d::Vec2(rect.getCenterX(), platform.getMaxY() + rect.getHeight() / 2.0);
            } else {
                // left
                //std::cout << "Left collinion" << std::endl;
                return cocos2d::Vec2(platform.getMinX() - rect.getWidth() / 2.0, rect.getCenterY());
            }
        } else {
            // top right
            if (topDiff < rightDiff) {
                //std::cout << "Top collinion" << std::endl;
                return cocos2d::Vec2(rect.getCenterX(), platform.getMaxY() + rect.getHeight() / 2.0);
            } else {
                //std::cout << "Right collinion" << std::endl;
                return cocos2d::Vec2(platform.getMaxX() + rect.getWidth() / 2.0, rect.getCenterY());
            }
        }
    } else {
        // collision on bottom.
        if (leftDiff < rightDiff) {
            // bottom left
            if (bottomDiff < leftDiff) {
                //std::cout << "Bottom collision" << std::endl;
                return cocos2d::Vec2(rect.getCenterX(), platform.getMinY() - rect.getHeight() / 2.0);
            } else {
                //std::cout << "Left collision" << std::endl;
                return cocos2d::Vec2(platform.getMinX() - rect.getWidth() / 2.0, rect.getCenterY());
            }
        } else {
            if (bottomDiff < rightDiff) {
                //std::cout << "Bottom collision" << std::endl;
                return cocos2d::Vec2(rect.getCenterX(), platform.getMinY() - rect.getHeight() / 2.0);
            } else {
                //std::cout << "Right collision" << std::endl;
                return cocos2d::Vec2(platform.getMaxX() + rect.getWidth() / 2.0, rect.getCenterY());
            }
        }
    }
}

void Character::move(float deltaTime, std::vector<Platform> platforms) {
    auto position = ani->getPosition();
    position.x += velocities.x * _maxVelocities.x * deltaTime;
    position.y += velocities.y * _maxVelocities.y * deltaTime;
    if (currentState == State::MID_AIR) {
        velocities.y -= (_gravity * deltaTime) / _maxVelocities.y;
    }
    bool collision = false;
    Platform collidedPlat;
    JTTW::Rectangle me(position.x, position.y, dimensions.getWidth(), dimensions.getHeight());
    for (auto plat = platforms.begin(); plat != platforms.end(); plat++) {
        if (isInCollision((*plat).getCollisionBounds(), me)) {
            collision = true;
            collidedPlat = *plat;
            break;
        }
    }
    
    //if (position.y < 0.0) {
    //    position.y = 0.0;
    //    currentState = State::STANDING;
    //    velocities.y = 0.0;
    //    updateAnimation();
    //} else
    if (collision == true) {
        // push to out of box
        position = collidedPosition(collidedPlat.getCollisionBounds(), me, velocities);
        JTTW::Rectangle r = collidedPlat.getCollisionBounds();
        if (me.getCenterX() + me.getWidth()/ 2.0 > r.getMinX() && me.getCenterX() - me.getWidth() / 2.0 < r.getMaxX()) {
            velocities.y = 0.0;
            if (me.getCenterY() > r.getMaxY() && currentState != State::STANDING) {
                std::cout << "current State: " << currentState << ", standing == " << State::STANDING << std::endl;
                currentState = State::STANDING;
                std::cout << "Calling from collision" << std::endl;
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
            if (isDirectlyAbove((*plat).getCollisionBounds(), me)) {
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
    dimensions.setCenter(position.x, position.y);
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
        ani->setTimeScale(0.9);
    }
}
