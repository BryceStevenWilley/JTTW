#include "Monk.hpp"
#include "Resolution.hpp"
#include "SimpleAudioEngine.h"

using namespace JTTW;

const double Monk::JUMP_INIT = ideal2Res(500);

Monk::Monk(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Monk", cocos2d::PhysicsMaterial(0.8, 0.0, 1.0), startPosition, dimensions, 0.7) {}

void Monk::initJump() {
    Character::initJump(JUMP_INIT);
}

void Monk::characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) {

}

void Monk::callHey() {
    Character::callHey();
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Sound/hey.wav");
}
