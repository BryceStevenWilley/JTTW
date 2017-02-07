//
//  AiActions.hpp
//  JTTW
//
//  Created by Bryce Willey on 1/19/17.
//
//

#ifndef AiActions_hpp
#define AiActions_hpp

#include "Character.hpp"

namespace JTTW {
/**
 * A class that represents when something should happen.
 * Currently supports when you arrive at a location, and a certain time from now.
 * TODO: fix this piece of shit, I tried to use a union but it failed horribly, 
 *   and using a boolean to determine which variable to use is disguisting.
 */
 
struct Trigger {
    bool usePosition;
    cocos2d::Vec2 position;
    float remainingTime;
    
    /**
     * The timer constructor: in time seconds from now, trigger.
     */
    Trigger(float time) : usePosition(false), position(0, 0), remainingTime(time) {}
    
    /**
     * The location constructor: at x location from now, trigger.
     */
    Trigger(float x, float y) : usePosition(true), position(x, y), remainingTime(0.0) {}
    
    bool isAtLocation(cocos2d::Vec2 p) {
        return isAtLocation(p.x, p.y);
    }
    
    bool isAtLocation(float x, float y) {
        bool xInTolerance = x > position.x - positionTolerance &&
                x < position.x + positionTolerance;
        bool yInTolerance = y > position.y - positionTolerance &&
                y < position.y + positionTolerance;
        return xInTolerance && yInTolerance;
    }
    
private:
    float positionTolerance = 50;

};
    
/**
 * Combines the action (see Character.h) and a trigger for that action into
 * a single struct.
 */
struct ActionAndTrigger {
    const Action a;
    Trigger trigger;

    ActionAndTrigger(const Action action, const float time) : a(action), trigger(time) {}
    ActionAndTrigger(const Action action, const Trigger &at) : a(action), trigger(at) {}
    ActionAndTrigger(const Action action, const float x, const float y) : a(action), trigger(x, y) {}
};
}
#endif /* AiActions_hpp */
