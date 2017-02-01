#ifndef AiAgent_hpp
#define AiAgent_hpp

#include <stdio.h>
#include "Character.h"
#include "AiActions.hpp"
#include <map>

namespace JTTW {

/**
 * A class that takes as input the other chacacters and the map (plan and setMap)
 * and produces a series of actions and when to execute them as output (plannedActions).
 * It can also choose which behavior is appropiate at which time.
 *
 * To create another possible behavior for the characters, add a private method named ____Behavior,
 * that returns a std::queue<ActionAndTrigger> and takes a Character * and std::vector<Character *> as parameters.
 * 
 * To create another unique character (Monk, Monkey, etc), extend this class.
 */
class AiAgent {
public:
    // A function pointer to a possible behavior that this agent can be following at the moment.
    typedef std::queue<ActionAndTrigger> (AiAgent::*Behavior)(Character *player, std::vector<Character *> otherCharacters);
    
    AiAgent(Character *controlledCharacter);
    ~AiAgent();
    
    void cedeToPlayer(AiAgent *previousPlayer);
    void retakeFromPlayer(AiAgent *nextPlayer);
    void plan(std::vector<Character *> otherCharactrs, cocos2d::EventKeyboard::KeyCode code, bool pressed);
    void plan(Character *player, std::vector<Character *> otherCharacters, cocos2d::EventKeyboard::KeyCode code, bool pressed);
    void changeBehavior(cocos2d::EventKeyboard::KeyCode code);
    void executePlan(float delta);
    void setMap(); // TODO: when the map API is created, pass the object that holds the map to this.
    
    Character *_controlledCharacter;
    
protected:
    // The queue of actions that will be executed in the future, according to their triggers.
    std::queue<ActionAndTrigger> plannedActions;
    Behavior _currentBehavior;

private:
    // The possible behaivors of the AI. All of these methods are implemented in AiAgentBehaivors.cpp.
    std::queue<ActionAndTrigger> stationaryBehavior(Character *player, std::vector<Character *> otherCharacters);
    std::queue<ActionAndTrigger> syncronizedBehavior(Character *player, std::vector<Character *> otherCharacters);
};
    
};



#endif /* AiAgent_hpp */
