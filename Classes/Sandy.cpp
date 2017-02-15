//
//  Sandy.cpp
//  JTTW
//
//  Created by Bryce Willey on 2/15/17.
//
//

#include "Sandy.hpp"

using namespace JTTW;

Sandy::Sandy(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Sandy", cocos2d::PhysicsMaterial(1.0, 0.0, 1.0), startPosition, dimensions) {}

void Sandy::jump() {
    Character::jump(300);
}
