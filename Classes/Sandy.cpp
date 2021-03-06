#include "Sandy.hpp"
#include "Resolution.hpp"
#include "SimpleAudioEngine.h"

using namespace JTTW;

const double Sandy::JUMP_INIT = ideal2Res(530);

Sandy::Sandy(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Sandy", cocos2d::PhysicsMaterial(1.0, 0.0, 1.0), startPosition, dimensions, 1.0) {}

void Sandy::initJump() {
    Character::initJump(JUMP_INIT);
}

void Sandy::characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) {

}

void Sandy::callHey() {
    Character::callHey();
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Sound/heySandy.mp3");
}
