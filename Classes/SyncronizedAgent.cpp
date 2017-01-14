#include "SyncronizedAgent.hpp"

using namespace JTTW;

void SyncronizedAgent::plan(Character *player, std::vector<Character *> otherCharacters) {
    // Literally, plan to do what the player is doing.
    if (player == _controlledCharacter) {
        return;
    }
    if (player->isMovingLeft() && !_controlledCharacter->isMovingLeft()) {
        plannedActions.push(ActionAndTrigger(Action::STOP, 0.0));
        plannedActions.push(ActionAndTrigger(Action::MOVE_LEFT, 0.0));
    } else if (player->isMovingRight() && !_controlledCharacter->isMovingRight()) {
        plannedActions.push(ActionAndTrigger(Action::STOP, 0.0));
        plannedActions.push(ActionAndTrigger(Action::MOVE_RIGHT, 0.0));
    } else if (player->getCurrentState() == Character::State::MID_AIR) { // TODO: causes agents to jump more than they should, fix
        plannedActions.push(ActionAndTrigger(Action::JUMP, 0.0));
    }
    if (!player->isMovingLeft() && !player->isMovingRight()) {
        plannedActions.push(ActionAndTrigger(Action::STOP, 0.0));
    }
}

void SyncronizedAgent::executePlan(float delta) {
    while(!plannedActions.empty()) {
        ActionAndTrigger a = plannedActions.front();
        bool doAction = false;
        if (a.trigger.usePosition) {
            // TODO: add a tolerance to this positioning system.
            if (a.trigger.position.x == _controlledCharacter->sprite->getPosition().x &&
                a.trigger.position.y == _controlledCharacter->sprite->getPosition().y) {
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

void SyncronizedAgent::setMap() {
    
}
