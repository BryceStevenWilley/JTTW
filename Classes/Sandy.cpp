#include "Sandy.hpp"

using namespace JTTW;

Sandy::Sandy(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Sandy", cocos2d::PhysicsMaterial(0.8, 0.0, 0.5), startPosition, dimensions) {}


void Sandy::jump() {
    Character::jump(450);
}


void Sandy::characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) {

}
