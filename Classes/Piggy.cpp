#include "Piggy.hpp"
#include "Collisions.hpp"
#include "Resolution.hpp"
#include "Boulder.hpp"
#include "SimpleAudioEngine.h"

using namespace JTTW;

const double Piggy::JUMP_INIT = ideal2Res(580);

Piggy::Piggy(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Piggy", cocos2d::PhysicsMaterial(0.8, 0.0, .8), startPosition, dimensions, 0.8) {}

void Piggy::impulseLeft(double deltaVel) {
    Character::impulseLeft(deltaVel);
}

void Piggy::impulseRight(double deltaVel) {
    Character::impulseRight(deltaVel);
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
            _state = BOULDER_MODE;
            //this->setAnimation(0, "powerstance", false);
        } else {
            _state = NORMAL;
            launchHeldCharacters();
            this->setAnimation(0, "powerstance", false);
            this->addAnimation(0, "push", false);
            this->addAnimation(0, "idle", true);
        }
        std::cout << (int) _state << std::endl;
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
        c->body->applyImpulse(cocos2d::Vec2(0.0, 770) * c->getMass());
    }
}

bool Piggy::shouldBeControlled() {
    return Character::shouldBeControlled() && _state != Piggy::BOULDER_MODE;
}

void Piggy::callHey() {
    Character::callHey();
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Sound/heyPiggy.wav");
}
