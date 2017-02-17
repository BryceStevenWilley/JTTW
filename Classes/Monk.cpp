//
//  Monk.cpp
//  JTTW
//
//  Created by Bryce Willey on 2/15/17.
//
//

#include "Monk.hpp"

using namespace JTTW;

Monk::Monk(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Monk", cocos2d::PhysicsMaterial(1.0, 0.0, 1.0), startPosition, dimensions) {}

void Monk::jump() {
    Character::jump(300);
}


void Monk::characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) {

}
