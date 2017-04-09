#ifndef SinkObject_hpp
#define SinkObject_hpp

#include <iostream>
#include "cocos2d.h"
#include "Moveable.hpp"

namespace JTTW {

/**
 * An object that will smash and squish the player. Think
 * Thwomp.
 */
class SinkObject : public Moveable {
public:
    enum State {
        PEAK, // At the top point.
        TROUGH, // At the bottom point.
        SINKING,
        FLOATING // Moving quickly to  hit the player.
    };
    
    SinkObject(std::string &fileName, cocos2d::Vec2 centerHit, cocos2d::Vec2 centerHold, cocos2d::Size imageSize, std::vector<cocos2d::Vec2> points, std::vector<double> frictions,
        std::vector<bool> deadlySides, double velocity, double pauseTime);
    
    virtual void move(float deltaTime);
    
    void landedCallback();
    void removeCallback();
    
private:
    State _currentState = State::PEAK;
    
    double _vel;
};

}



#endif /* SinkObject_hpp */
