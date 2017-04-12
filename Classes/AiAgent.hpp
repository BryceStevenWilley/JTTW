#ifndef AiAgent_hpp
#define AiAgent_hpp

#include <stdio.h>
#include "Character.hpp"
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
    typedef void (AiAgent::*Behavior)(Character *player, std::vector<Character *> otherCharacters);
    
    AiAgent(Character *controlledCharacter);
    
    void switchUserToHereFrom(AiAgent *previousPlayer);
    void retakeFromUser(AiAgent *nextPlayer);
    void adjustOffset(Character *player);
    void plan(std::vector<Character *> otherCharactrs, cocos2d::EventKeyboard::KeyCode code, bool pressed);
    void plan(Character *player, std::vector<Character *> otherCharacters);
    void changeBehavior(Character *player, cocos2d::EventKeyboard::KeyCode code, bool pressed);
    void executeControl(float delta);
    
    cocos2d::Vec2 getPlayerPosOffset() const;
    void setPlayerPosOffset(cocos2d::Vec2 playerPosOffset);
    
    Character *_controlledCharacter;
protected:
    cocos2d::Vec2 _playerPosOffset = cocos2d::Vec2(0, 0);

    // The queue of actions that will be executed in the future, according to their triggers.
    Behavior _currentBehavior;
    
    // The input 'trajectory' to match.
    cocos2d::Vec2 desiredVel;
    cocos2d::Vec2 desiredPosition;
    
    // Control parameters.
    double kp = 16.0;
    double kv = 20.0;
    double b;
    double k;

private:
    // The possible behaivors of the AI. All of these methods are implemented in AiAgentBehaivors.cpp.
    void followBehavior(Character *player, std::vector<Character *> otherCharacters);
    void stationaryBehavior(Character *player, std::vector<Character *> otherCharacters);
    void goToPointBehavior(Character *player, std::vector<Character *> otherCharacters);
    
    cocos2d::Vec2 goToPoint;
};
    
};

#endif /* AiAgent_hpp */
