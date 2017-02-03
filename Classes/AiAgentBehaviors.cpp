// More implementations for the AiAgent class.
//
// This is seperate from AiAgent.cpp because this will eventually
// get to be very large, containing all possible behaviors for all
// characters.

#include "AiAgent.hpp"

using namespace JTTW;

/**
 * Copies exactly what the player character is doing at the moment.
 * 
 * Player just jumped? You jump. Player moved left? You move left.
 * For now, just a simple AI for making characters follow player.
 */
std::queue<ActionAndTrigger> AiAgent::syncronizedBehavior(Character *player, std::vector<Character *> otherCharacters) {
    std::queue<ActionAndTrigger> plannedActions;
    // Literally, plan to do what the player is doing.
    if (player == _controlledCharacter) {
        return plannedActions;
    }
    if (player->isMovingLeft() && !_controlledCharacter->isMovingLeft()) {
        plannedActions.push(ActionAndTrigger(Action::STOP, 0.0));
        plannedActions.push(ActionAndTrigger(Action::MOVE_LEFT, 0.0));
    } else if (player->isMovingRight() && !_controlledCharacter->isMovingRight()) {
        plannedActions.push(ActionAndTrigger(Action::STOP, 0.0));
        plannedActions.push(ActionAndTrigger(Action::MOVE_RIGHT, 0.0));
    } else if (player->getCurrentState() == Character::State::MID_AIR && player->justJumped()) { // TODO: causes agents to jump more than they should, fix
        plannedActions.push(ActionAndTrigger(Action::JUMP, 0.0));
    }
    if (!player->isMovingLeft() && !player->isMovingRight()) {
        plannedActions.push(ActionAndTrigger(Action::STOP, 0.0));
    }
    return plannedActions;
}

/**
 * Stand in the same place. Always chooses the stop action, no matter what.
 */
std::queue<ActionAndTrigger> AiAgent::stationaryBehavior(Character *player, std::vector<Character *> otherCharacters) {
    std::queue<ActionAndTrigger> plannedActions;
    // Literally, stand completely still.
    plannedActions.push(ActionAndTrigger(Action::STOP, 0.0));
    return plannedActions;
}
