#include "Piggy.hpp"
#include "Collisions.hpp"
#include "Resolution.hpp"
#include "Boulder.hpp"

using namespace JTTW;

const double Piggy::JUMP_INIT = ideal2Res(500);

Piggy::Piggy(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Piggy", cocos2d::PhysicsMaterial(0.8, 0.0, .8), startPosition, dimensions) {}


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

void Piggy::initJump() {
    if (_state != BOULDER_MODE) {
        Character::initJump(JUMP_INIT);
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
            //this->setTag(0);
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
            //this->setTag(CHARACTER_TAG);
            alonecrown->setRotation(0);
            followcrown->setRotation(0);
            launchHeldCharacters();
        }
    }
}

void Piggy::addHeldCharacter(Character *c) {
    std::cout << "Piggy is holding " << c->characterName << std::endl;
    _heldCharacters.push_back(c);
}

void Piggy::removeHeldCharacter(Character *c) {
    auto x = std::find(_heldCharacters.begin(), _heldCharacters.end(), c);
    if (x != _heldCharacters.end()) {
        _heldCharacters.erase(x);
    }
}

void Piggy::launchHeldCharacters() {
    for (Character *c: _heldCharacters) {
        std::cout << "Launching " << c->characterName << std::endl;
        c->body->applyImpulse(cocos2d::Vec2(0.0, 600) * c->getMass());
    }
}
