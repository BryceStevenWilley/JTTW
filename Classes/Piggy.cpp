#include "Piggy.hpp"
#include "Collisions.hpp"
#include "Resolution.hpp"
#include "Boulder.hpp"

using namespace JTTW;

const double Piggy::JUMP_INIT = ideal2Res(580);

Piggy::Piggy(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Piggy", cocos2d::PhysicsMaterial(0.8, 0.0, .8), startPosition, dimensions, 0.8) {}

void Piggy::impulseLeft(double deltaVel) {
    if (_state != BOULDER_MODE) {
        Character::impulseLeft(deltaVel);
    }
}

void Piggy::impulseRight(double deltaVel) {
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
    if (code == cocos2d::EventKeyboard::KeyCode::KEY_D) {
        std::cout << "Changing mode to ";
        if (pressed) {
            Character::stop();
            //body->setCategoryBitmask((int)CollisionCategory::Boulder);
            //body->setCollisionBitmask((int)CollisionCategory::ALL);
            //body->setContactTestBitmask((int)CollisionCategory::ALL);
            //this->setTag(0);
            _state = BOULDER_MODE;
            alonecrown->setRotation(3.1415926/2);
            alonecrown->setDirty(true);
            followcrown->setRotation(3.1415926/2);
            followcrown->setDirty(true);
            this->setAnimation(0, "powerstance", false);
        } else {
            //body->setCategoryBitmask((int)CollisionCategory::Character);
            //body->setCollisionBitmask((int)CollisionCategory::PlatformAndBoulder);
            //body->setContactTestBitmask((int)CollisionCategory::PlatformAndBoulder);
            _state = NORMAL;
            //this->setTag(CHARACTER_TAG);
            alonecrown->setRotation(0);
            followcrown->setRotation(0);
            launchHeldCharacters();
            this->setAnimation(0, "powerstance", false);
            this->addAnimation(0, "push", false);
            this->addAnimation(0, "idle", true);

        }
        std::cout << (int) _state << std::endl;
    }
}

void Piggy::addHeldCharacter(Character *c) {
    //if (_state == BOULDER_MODE) {
        std::cout << "Piggy is holding " << c->characterName << std::endl;
        _heldCharacters.push_back(c);
    //}
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
        c->body->applyImpulse(cocos2d::Vec2(0.0, 770) * c->getMass());
    }
}

bool Piggy::shouldBeControlled() {
    return Character::shouldBeControlled() && _state != Piggy::BOULDER_MODE;
}
