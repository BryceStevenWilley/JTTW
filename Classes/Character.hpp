//
//  PCharacter.hpp
//  JTTW
//
//  Created by Bryce Willey on 2/3/17.
//
//

#ifndef PCharacter_hpp
#define PCharacter_hpp

#include <stdio.h>
#include <spine/spine-cocos2dx.h>
#include "cocos2d.h"

namespace JTTW {

/**
 * The possible actions that a character can do. Should correspond 1:1
 * to methods in the character class.
 */
enum Action {
    STOP,
    MOVE_LEFT,
    MOVE_RIGHT,
    JUMP
};

// Forward declaration of Platform.
class Platform;


const int CHARACTER_TAG = 14;

/**
 * @brief Class for interactive characters based on physics.
 *
 * DON'T ACCESS POSITION OR VELOCITY UNLESS AT BEGINNING OF LEVEL.
 * Instead, access through the impluse functions.
 */
class Character : public spine::SkeletonAnimation {
public:
    enum State {
        STANDING,
        MID_AIR
    };
    
    Character(const std::string artFileName, cocos2d::PhysicsMaterial mat, cocos2d::Vec2 startPosition, cocos2d::Size dimensions);

    cocos2d::PhysicsBody *body;
    
    // Applies an impulse going to the left of the character.
    //     deltaVel - the amount that the velocity going to the left should change.
    virtual void impulseLeft(float deltaVel);
    
    // Applies an impulse going to the right of the character.
    //     deltaVel - the amount that the velocity going to the right should change.
    virtual void impulseRight(float deltaVel);
    
    void rebalanceImpulse(); 
    
    // Stops the character by applying an impulse that sets the velocity to 0.
    virtual void stop();
    
    // Allows the character to jump.
    void jump();
    
    bool isMovingLeft() const;
    bool isMovingRight() const;
    
    void landedCallback();
    
    void transferVelocity(Character *reciever);
    
    bool justJumped() const;
    const State getCurrentState() const;
    
    const std::string characterName;
    
    float getMass() const;
    
    cocos2d::Sprite *crown;
    void restartFromStart();

private:
    void updateAnimation();
       
    cocos2d::Vec2 _startingPosition;
    
    State _currentState = State::STANDING;
    
    double leftMomentum = 0.0;
    double rightMomentum = 0.0;

};
}

#endif /* PCharacter_hpp */
