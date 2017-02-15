//
//  Monkey.cpp
//  JTTW
//
//  Created by Bryce Willey on 2/15/17.
//
//

#include "Monkey.hpp"

using namespace JTTW;

Monkey::Monkey(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Monkey", cocos2d::PhysicsMaterial(1.0, 0.0, 1.0), startPosition, dimensions) {}


void Monkey::jump() {
    Character::jump(350);
}
