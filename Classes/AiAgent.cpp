#include <stdio.h>
#include "AiAgent.hpp"

using namespace JTTW;

AiAgent::AiAgent(Character *controlledCharacter) :
_controlledCharacter(controlledCharacter), _currentBehavior(&AiAgent::syncronizedBehavior) {}

AiAgent::~AiAgent() {}

void AiAgent::setMap() {
}

void AiAgent::plan(Character *player, std::vector<Character *> otherCharacters, cocos2d::EventKeyboard::KeyCode code) {
    if (code == cocos2d::EventKeyboard::KeyCode::KEY_A) {
        // switch behaviors to stationary.
        _currentBehavior = &AiAgent::stationaryBehavior;
    } else if (code == cocos2d::EventKeyboard::KeyCode::KEY_O) {
        _currentBehavior = &AiAgent::syncronizedBehavior;
    }
    
    if (player == _controlledCharacter) {
        return;
    }
    
    std::queue<ActionAndTrigger> toAdd  = (this->*_currentBehavior)(player, otherCharacters);
    
    // Put all of the returned ActionAndTriggers onto the master action queue.
    while (!toAdd.empty()) {
        ActionAndTrigger at = toAdd.front();
        toAdd.pop();
        plannedActions.push(at);
    }
}

void AiAgent::executePlan(float delta) {
    while(!plannedActions.empty()) {
        ActionAndTrigger a = plannedActions.front();
        bool doAction = false;
        if (a.trigger.usePosition) {
            // TODO: add a tolerance to this positioning system.
            if (a.trigger.position.x == _controlledCharacter->ani->getPosition().x &&
                a.trigger.position.y == _controlledCharacter->ani->getPosition().y) {
                doAction = true;
            }
        } else {
            if (a.trigger.remainingTime <= 0.0) {
                doAction = true;
            } else {
                plannedActions.pop();
                a.trigger.remainingTime -= delta;
                plannedActions.push(a);
            }
        }
        if (doAction == true) {
            switch(a.a) {
                case Action::STOP:
                    _controlledCharacter->stop();
                    break;
                case Action::MOVE_LEFT:
                    _controlledCharacter->accelerateLeft(1.0);
                    break;
                case Action::MOVE_RIGHT:
                    _controlledCharacter->accelerateRight(1.0);
                    break;
                case Action::JUMP:
                    _controlledCharacter->jump(1.0);
                    break;
                default:
                    // do nothing
                    break;
            }
            plannedActions.pop();
        } else {
            break;
        }
    }
}
