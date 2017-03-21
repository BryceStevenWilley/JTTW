#include "Sandy.hpp"

using namespace JTTW;

const double Sandy::JUMP_INIT = 500;

Sandy::Sandy(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Sandy", cocos2d::PhysicsMaterial(0.8, 0.0, 0.5), startPosition, dimensions) {}


void Sandy::initJump() {
    Character::initJump(JUMP_INIT);
}


void Sandy::characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) {

}
