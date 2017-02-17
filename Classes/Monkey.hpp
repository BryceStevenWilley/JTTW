#ifndef Monkey_hpp
#define Monkey_hpp

#include <iostream>
#include "Character.hpp"

namespace JTTW {
class Monkey : public Character {
public:
    enum State {
        NORMAL,
        CLIMBING
    };

    Monkey(cocos2d::Vec2 startPosition, cocos2d::Size dimensions);
    
    virtual void impulseLeft(float deltaVel) override;
    virtual void impulseRight(float deltaVel) override;
    virtual void jump() override;
    virtual void characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) override;
    
    void enteringClimeable();
    void leavingClimeable();
    
private:
    void updateClimbingVel();

    State _state;
    
    double climbUpVel;
    double climbDownVel;
};
}

#endif /* Monkey_hpp */
