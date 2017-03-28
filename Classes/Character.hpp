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
    JUMP,
    FREEZE,
    FALL
};

const int CHARACTER_TAG = 10;

/**
 * @brief Class for interactive characters based on physics.
 *
 * DON'T ACCESS POSITION OR VELOCITY UNLESS AT BEGINNING OF LEVEL.
 * Instead, access through the impluse functions.
 */
class Character : public spine::SkeletonAnimation {
public:
    const static double JUMP_DECAY;
    const static double JUMP_INIT_FRACTION;
    const static double VEL_LIMIT;
    const static double CROWN_SCALE;

    enum State {
        STANDING,
        MID_AIR,
        FROZEN,
    };
    
    static Character *createFromName(const std::string name, cocos2d::Vec2 startPosition, cocos2d::Size dimensions);
    
    Character(const std::string artFileName, cocos2d::PhysicsMaterial mat, cocos2d::Vec2 startPosition, cocos2d::Size dimensions);

    cocos2d::PhysicsBody *body;
    
    // Applies an impulse going to the left of the character.
    //     deltaVel - the amount that the velocity going to the left should change.
    virtual void impulseLeft(float deltaVel);
    virtual void impulseLeftButNoRebalance(float deltaVel);
    
    // Applies an impulse going to the right of the character.
    //     deltaVel - the amount that the velocity going to the right should change.
    virtual void impulseRight(float deltaVel);
    virtual void impulseRightButNoRebalance(float deltaVel);
    
    void applyForceRight(double fprimex);
    
    void rebalanceImpulse(); 
    
    // Stops the character by applying an impulse that sets the velocity to 0.
    virtual void stop();

    void freeze();
    
    // Allows the character to jump.
    virtual void initJump() = 0;
    virtual void stopJump();
    void jumpFromForce(double fprime_y);
    
    virtual void characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) = 0;
    
    bool isMovingLeft() const;
    bool isMovingRight() const;
    
    void landedCallback(cocos2d::PhysicsBody *plat, cocos2d::Vec2 newRightDir);
    void leftCallback(cocos2d::PhysicsBody *plat);
    void wallHitCallback(cocos2d::PhysicsBody *wall);
    void wallLeftCallback(cocos2d::PhysicsBody *wall);
    
    void transferVelocity(Character *reciever);
    
    bool justJumped() const;
    const State getCurrentState() const;
    
    const std::string characterName;
    
    float getMass() const;
    cocos2d::Size getSize();
    
    cocos2d::Sprite *followcrown;
    cocos2d::Sprite *alonecrown;
    
    cocos2d::Sprite *currentCrown;
    virtual void restartFromRespawn();
    void setNewRespawn(cocos2d::Vec2 newRespawn);
    double getRespawnProgress() const;
    
    void updateLoop(float delta);
    virtual void continueMotion();

    // TODO: TODO: BAD DESIGN, WHY DOES CHARACTER HAVE TO KNOW ABOUT AI THINGS TO UPDATE ANIMATION CORRECTLY?
    void toggleToAI();
    void toggleToPlayer();
    
    void setToRespawn();
    bool _respawnNextCycle = false;

protected:
    void initJump(double force);
    State _currentState = State::STANDING;
        
    double leftMomentum = 0.0;
    double rightMomentum = 0.0;
    int wallsHit = 0;
    
    
private:
    void updateAnimation(State oldState);
       
    cocos2d::Vec2 _respawnPosition;

    cocos2d::Vec2 _oldVel;
    
    int platformsStandingOn = 0;

    cocos2d::Size _dimensions;

    double _frozenTimer = 0.0;
    
    cocos2d::Vec2 _rightVector = cocos2d::Vec2(1, 0);
    
    double jumpForce = 0.0;
    
    bool aiControl = true;
};
}

#endif /* PCharacter_hpp */
