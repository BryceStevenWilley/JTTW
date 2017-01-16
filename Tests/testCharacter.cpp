//
//  testCharacter.cpp
//  JTTW
//
//  Created by Bryce Willey on 1/16/17.
//
//

#include <stdio.h>

#include "catch.hpp"

#include "Character.h"

using namespace JTTW;

TEST_CASE("Testing Character") {
    Character c("image.png", cocos2d::Vec2(40, 40), cocos2d::Vec2(400, 400), 4);
    
    SECTION("Checking velocity changes") {
        // Should start off stopped.
        REQUIRE(!c.isMovingLeft());
        REQUIRE(!c.isMovingRight());
        
        // Move left.
        c.accelerateLeft(1.0);
        REQUIRE(c.isMovingLeft());
        
        // Stop again.
        c.stop();
        REQUIRE(!c.isMovingLeft());
        REQUIRE(!c.isMovingRight());
        
        
        // Move right.
        c.accelerateRight(1.0);
        REQUIRE(c.isMovingRight());
    }
}
