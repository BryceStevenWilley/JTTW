#include <iostream>
#include "AiAgent.hpp"

using namespace JTTW;

AiAgent::AiAgent(Character *controlledCharacter) :
_controlledCharacter(controlledCharacter), _currentBehavior(&AiAgent::stationaryBehavior) {}

AiAgent::~AiAgent() {}

void AiAgent::setMap() {
}

void AiAgent::cedeToPlayer(AiAgent *previousPlayer) {
    // clean up any movements from the AI.
    if (_currentBehavior == &AiAgent::stationaryBehavior) {
        // Need to transfer velocity.
        previousPlayer->_controlledCharacter->transferVelocity(_controlledCharacter);
    } else if (_currentBehavior == &AiAgent::syncronizedBehavior) {
        // No need to transfer velocity.
    }
    
}

void AiAgent::retakeFromPlayer(AiAgent *nextPlayer) {
    
}

void AiAgent::plan(std::vector<Character *> otherCharacters, cocos2d::EventKeyboard::KeyCode code, bool pressed) {
    if (pressed) {
        changeBehavior(code);
    }
    // All code to control goes here.
    using KeyCode = cocos2d::EventKeyboard::KeyCode;
    
    switch(code) {
        case KeyCode::KEY_LEFT_ARROW:
            if (pressed) {
                _controlledCharacter->impulseLeft(200.0);
            } else { // released
                _controlledCharacter->impulseLeft(-200.0);
            }
            break;
        case KeyCode::KEY_RIGHT_ARROW:
            if (pressed) {
                _controlledCharacter->impulseRight(200.0);
            } else {
                _controlledCharacter->impulseRight(-200.0);
            }
            break;
        case KeyCode::KEY_SPACE:
            _controlledCharacter->jump();
            break;
        default:
            // do nothing
            break;
    }
    return;

}

void AiAgent::plan(Character *player, std::vector<Character *> otherCharacters, cocos2d::EventKeyboard::KeyCode code, bool pressed) {
    if (pressed) {
        changeBehavior(code);
    }
    
    if (player == _controlledCharacter) {
        // ERROR!!!!
        std::cout << "ERROR: calling wrong function for ai controlled character" << std::endl;
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

void AiAgent::changeBehavior(cocos2d::EventKeyboard::KeyCode code) {
    if (code == cocos2d::EventKeyboard::KeyCode::KEY_A) { // 'A' is becoming a toggle.
        if (_currentBehavior == &AiAgent::stationaryBehavior) {
            _currentBehavior = &AiAgent::syncronizedBehavior;
            std::cout << "Behavior is now syncronized" << std::endl;
        } else if (_currentBehavior == &AiAgent::syncronizedBehavior) {
            _currentBehavior = &AiAgent::stationaryBehavior;
            std::cout << "Behavior is now stationary" << std::endl;
        }
    }
}

void AiAgent::executePlan(float delta) {
    while(!plannedActions.empty()) {
        ActionAndTrigger a = plannedActions.front();
        bool doAction = false;
        if (a.trigger.usePosition) {
            // TODO: add a tolerance to this positioning system.
            if (a.trigger.position.x == _controlledCharacter->getPosition().x &&
                a.trigger.position.y == _controlledCharacter->getPosition().y) {
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
                    _controlledCharacter->impulseLeft(200.0);
                    break;
                case Action::MOVE_RIGHT:
                    _controlledCharacter->impulseRight(200.0);
                    break;
                case Action::JUMP:
                    _controlledCharacter->jump();
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
