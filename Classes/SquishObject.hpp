#ifndef SquishObject_hpp
#define SquishObject_hpp

#include <iostream>
#include "cocos2d.h"
#include "Moveable.hpp"

namespace JTTW {

/**
 * An object that will smash and squish the player. Think
 * Thwomp.
 */
class SquishObject : public Moveable {
public:
    enum State {
        PREP, // i.e. Moving up slowly
        HOLD, // i.e. Staying still at the top of the motion.
        ATTACK // Moving quickly to  hit the player.
    };
    
    SquishObject(std::string &fileName, cocos2d::Vec2 centerHit, cocos2d::Vec2 centerHold, cocos2d::Size imageSize, std::vector<cocos2d::Vec2> points, std::vector<double> frictions,
        std::vector<bool> deadlySides, double slowVelocity, double highVelocity, double pauseTime);
    
    virtual void move(float deltaTime);
    
private:
    State _currentState = State::PREP;
    
    double _vel, _quickVel;
    double holdTime;
    
    double accumHeldTime;
};

}

#endif /* SquishObject_hpp */
