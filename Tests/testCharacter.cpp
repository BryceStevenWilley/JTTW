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
    Character c("spineboy", cocos2d::Vec2(40, 40), cocos2d::Vec2(400, 400), 4);
    
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
    
    SECTION("Gravity/ update tests") {
        REQUIRE(c.getYVelocity() == 0.0);
        
        c.jump(1.0); // Should be exactly at 400px now.
        REQUIRE(c.getYVelocity() == 400.0);
        
        // Half a second later, velocitity should be 400 - 1/2 * 4.
        c.move(.5);
        REQUIRE(c.getYVelocity() == 398.0);
    }
    
}
