#ifndef Monkey_hpp
#define Monkey_hpp

#include <iostream>
#include "Character.hpp"
#include "Platform.hpp"
#include "Vine.hpp"

namespace JTTW {
class Monkey : public Character {
public:
    static const double JUMP_INIT;
    static const double CLIMB_VEL;
    static const double VINE_CLIMB_INC;
    
    enum State {
        NORMAL,
        CLIMBING,
        SWINGING
    };

    Monkey(cocos2d::Vec2 startPosition, cocos2d::Size dimensions);
    
    virtual void impulseLeft(float deltaVel) override;
    virtual void impulseRight(float deltaVel) override;
    virtual void initJump() override;
    virtual void characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) override;
    
    virtual void continueMotion() override;
    
    void enteringClimeable(cocos2d::PhysicsWorld *world, SceneObject *p, cocos2d::Vec2 offset, cocos2d::Vec2 upDir, bool alreadyOn);
    void leavingClimeable(bool goingToReattach, bool ignoreCount);
    
    void enteringVine(cocos2d::PhysicsWorld *world, SceneObject *vine, double offset, bool alreadyOn);
    void leavingVine(bool reattaching);
    
    virtual void restartFromRespawn() override;
    virtual void die(Character::CauseOfDeath cause) override;
    
    void setBoulderBury();
    void setBoulderUnbury();
    Monkey::State getMonkeyState();
    
    void setHangingCharacter(Character *c);
    bool hasHangingCharacter();
    
    virtual bool shouldBeControlled() override;
    
private:
    void updateClimbingVel();
    void moveAlongVine(float deltaP);
    void moveAlongClimbable(cocos2d::Vec2 up);

    State _state;
    
    double climbUpVel = 0.0;
    double climbDownVel = 0.0;
    
    cocos2d::PhysicsJoint* pinJoint = nullptr;
    cocos2d::PhysicsJoint* gearJoint = nullptr;
    SceneObject *currentAttached = nullptr;
    cocos2d::Vec2 currentAttachedOffset = cocos2d::Vec2::ZERO;
    cocos2d::PhysicsWorld *currentWorld;
    
    cocos2d::Vec2 _upDir;
    
    Character *_hangingCharacter = nullptr;
    
    int climbingCount = 0;
};
}

#endif /* Monkey_hpp */
