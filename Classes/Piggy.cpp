//
//  Piggy.cpp
//  JTTW
//
//  Created by Bryce Willey on 2/15/17.
//
//

#include "Piggy.hpp"
#include "Collisions.hpp"

using namespace JTTW;

Piggy::Piggy(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Piggy", cocos2d::PhysicsMaterial(1.0, 0.0, 1.0), startPosition, dimensions) {}


void Piggy::impulseLeft(float deltaVel) {
    if (_state != BOULDER_MODE) {
        Character::impulseLeft(deltaVel);
    }
}

void Piggy::impulseRight(float deltaVel) {
    if (_state != BOULDER_MODE) {
        Character::impulseRight(deltaVel);
    }
}

void Piggy::jump() {
    if (_state != BOULDER_MODE) {
        Character::jump(330);
    }
}

void Piggy::characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) {
    std::cout << "calling special with code " << (int)code << std::endl;
    std::cout << "S is " << (int)cocos2d::EventKeyboard::KeyCode::KEY_S << std::endl;
    if (code == cocos2d::EventKeyboard::KeyCode::KEY_S && pressed) {
        std::cout << "Changing mode!" << std::endl;
        if (_state != BOULDER_MODE) {
            Character::stop();
            body->setCategoryBitmask((int)CollisionCategory::Boulder);
            body->setCollisionBitmask((int)CollisionCategory::ALL);
            body->setContactTestBitmask((int)CollisionCategory::ALL);
            _state = BOULDER_MODE;
            alonecrown->setRotation(3.1415926/2);
            alonecrown->setDirty(true);
            followcrown->setRotation(3.1415926/2);
            followcrown->setDirty(true);
        } else {
            body->setCategoryBitmask((int)CollisionCategory::Character);
            body->setCollisionBitmask((int)CollisionCategory::PlatformAndBoulder);
            body->setContactTestBitmask((int)CollisionCategory::PlatformAndBoulder);
            _state = NORMAL;
            alonecrown->setRotation(0);
            followcrown->setRotation(0);
        }
    }
}
