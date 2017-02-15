//
//  Piggy.cpp
//  JTTW
//
//  Created by Bryce Willey on 2/15/17.
//
//

#include "Piggy.hpp"

using namespace JTTW;

Piggy::Piggy(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Piggy", cocos2d::PhysicsMaterial(1.0, 0.0, 1.0), startPosition, dimensions) {}

 
void Piggy::jump() {
    Character::jump(290);
}
