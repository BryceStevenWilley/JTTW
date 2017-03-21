#include "Monk.hpp"
#include "SimpleAudioEngine.h"

using namespace JTTW;

const double Monk::JUMP_INIT = 450;

Monk::Monk(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Monk", cocos2d::PhysicsMaterial(0.8, 0.0, 0.5), startPosition, dimensions) {}

void Monk::initJump() {
    Character::initJump(JUMP_INIT);
}


void Monk::characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) {
    if (pressed && code == cocos2d::EventKeyboard::KeyCode::KEY_S && reachiblePeg != nullptr) {
        reachiblePeg->triggerPeg(this->getPosition());
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Sound/BoulderCrumble.wav");
        reachiblePeg = nullptr;
    }
}

void Monk::addReachiblePeg(Peg *p) {
    std::cout << "Adding reachible peg." << std::endl;
    reachiblePeg = p;
}
