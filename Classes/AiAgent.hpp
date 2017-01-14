#ifndef AiAgent_hpp
#define AiAgent_hpp

#include <stdio.h>
#include "Character.h"

namespace JTTW {
    
enum Action {
    STOP,
    MOVE_LEFT,
    MOVE_RIGHT,
    JUMP
};
    
struct ActionTrigger { // TODO: fix this piece of shit.
    bool usePosition;
    cocos2d::Vec2 position;
    float remainingTime;
    
    ActionTrigger(float time) : usePosition(false), position(0, 0), remainingTime(time) {}
    ActionTrigger(float x, float y) : usePosition(true), position(x, y), remainingTime(0.0) {}
};
    
struct ActionAndTrigger {
    const Action a;
    ActionTrigger trigger;
    
    ActionAndTrigger(const Action action, const float time) : a(action), trigger(time) {}
    ActionAndTrigger(const Action action, const ActionTrigger &at) : a(action), trigger(at) {}
};
    
class AiAgent {
public:
    AiAgent(Character *controlledCharacter) : _controlledCharacter(controlledCharacter) {}
    virtual void plan(Character *player, std::vector<Character *> otherCharacters) = 0;
    virtual void executePlan(float delta) = 0;
    virtual void setMap() = 0; // TODO: when the map API is created, pass the object that holds the map to this.
    
//protected:
    Character *_controlledCharacter;
};
    
};



#endif /* AiAgent_hpp */
