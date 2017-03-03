#ifndef Monkey_hpp
#define Monkey_hpp

#include <iostream>
#include "Character.hpp"
#include "Vine.hpp"

namespace JTTW {
class Monkey : public Character {
public:
    enum State {
        NORMAL,
        CLIMBING,
        SWINGING
    };

    Monkey(cocos2d::Vec2 startPosition, cocos2d::Size dimensions);
    
    virtual void impulseLeft(float deltaVel) override;
    virtual void impulseRight(float deltaVel) override;
    virtual void jump() override;
    virtual void characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) override;
    
    void enteringClimeable();
    void leavingClimeable();
    void enteringVine(cocos2d::PhysicsWorld *world, Vine *vine, double offset, cocos2d::Vec2 collisionPoint);
    void leavingVine(cocos2d::PhysicsWorld *w);
    
private:
    void updateClimbingVel();
    void moveAlongVine(float deltaP);

    State _state;
    
    double climbUpVel = 0.0;
    double climbDownVel = 0.0;
    
    cocos2d::PhysicsJoint* pinJoint = nullptr;
    cocos2d::PhysicsJoint* gearJoint = nullptr;
    Vine *currentVine = nullptr;
    double currentVineOffset = 0.0;
    cocos2d::PhysicsWorld *currentWorld;
};
}

#endif /* Monkey_hpp */
