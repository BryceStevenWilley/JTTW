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
std::queue<ActionAndTrigger> AiAgent::syncronizedBehavior(Character *player, std::vector<Character *> otherCharacters, cocos2d::EventKeyboard::KeyCode playerAction) {
    std::queue<ActionAndTrigger> plannedActions;
    if (player == _controlledCharacter) {
        return plannedActions;
    }
    
    // Literally, plan to do what the player is doing.
    // Match the player's horizontal velocity.
    cocos2d::Vec2 vel = player->body->getVelocity();
    cocos2d::Vec2 myVel = _controlledCharacter->body->getVelocity();
    
    if (myVel.x > vel.x) {
        // I am traveling more right than player.
        _controlledCharacter->impulseLeft(myVel.x - vel.x);
    } else if (myVel.x < vel.x){
        _controlledCharacter->impulseRight(vel.x - myVel.x);
    }
    
    //_controlledCharacter->impulseLeft(_controlledCharacter->body->getVelocity().x - vel.x);
    //if (player->body-> && !_controlledCharacter->isMovingLeft() && playerAction != cocos2d::EventKeyboard::KeyCode::KEY_SPACE) {
    //    plannedActions.push(ActionAndTrigger(Action::STOP, 0.0));
    //    plannedActions.push(ActionAndTrigger(Action::MOVE_LEFT, 0.0));
    //} else if (player->isMovingRight() && !_controlledCharacter->isMovingRight() && playerAction != cocos2d::EventKeyboard::KeyCode::KEY_SPACE) {
    //    plannedActions.push(ActionAndTrigger(Action::STOP, 0.0));
    //    plannedActions.push(ActionAndTrigger(Action::MOVE_RIGHT, 0.0));
    //} else
     if (player->getCurrentState() == Character::State::MID_AIR && playerAction == cocos2d::EventKeyboard::KeyCode::KEY_SPACE) { // TODO: causes agents to jump more than they should, fix
        plannedActions.push(ActionAndTrigger(Action::JUMP, 0.0));
    }
    //if (!player->isMovingLeft() && !player->isMovingRight()) {
    //    plannedActions.push(ActionAndTrigger(Action::STOP, 0.0));
    //}
    return plannedActions;
}

/**
 * Stand in the same place. Always chooses the stop action, no matter what.
 */
std::queue<ActionAndTrigger> AiAgent::stationaryBehavior(Character *player, std::vector<Character *> otherCharacters, cocos2d::EventKeyboard::KeyCode playerAction) {
    std::queue<ActionAndTrigger> plannedActions;
    // Literally, stand completely still.
    plannedActions.push(ActionAndTrigger(Action::STOP, 0.0));
    return plannedActions;
}

std::queue<ActionAndTrigger> AiAgent::dumbFollowBehavior(Character *player, std::vector<Character *> otherCharacters, cocos2d::EventKeyboard::KeyCode playerAction) {
        std::queue<ActionAndTrigger> plannedActions;
    // Literally, plan to do what the player is doing, but at exactly the position
    // where they're doing it.
    if (player == _controlledCharacter) {
        return plannedActions;
    }
    cocos2d::Vec2 playerPosition = player->body->getPosition();
    if (player->isMovingLeft() && !_controlledCharacter->isMovingLeft() && playerAction != cocos2d::EventKeyboard::KeyCode::KEY_SPACE) {
        plannedActions.push(ActionAndTrigger(Action::STOP, playerPosition.x, playerPosition.y));
        plannedActions.push(ActionAndTrigger(Action::MOVE_LEFT, playerPosition.x, playerPosition.y));
    } else if (player->isMovingRight() && !_controlledCharacter->isMovingRight() && playerAction != cocos2d::EventKeyboard::KeyCode::KEY_SPACE) {
        plannedActions.push(ActionAndTrigger(Action::STOP, playerPosition.x, playerPosition.y));
        plannedActions.push(ActionAndTrigger(Action::MOVE_RIGHT, playerPosition.x, playerPosition.y));
    } else if (player->getCurrentState() == Character::State::MID_AIR && player->justJumped() && playerAction == cocos2d::EventKeyboard::KeyCode::KEY_SPACE) { // TODO: causes agents to jump more than they should, fix
        plannedActions.push(ActionAndTrigger(Action::JUMP, playerPosition.x, playerPosition.y));
    }
    if (!player->isMovingLeft() && !player->isMovingRight()) {
        plannedActions.push(ActionAndTrigger(Action::STOP, playerPosition.x, playerPosition.y));
    }
    return plannedActions;
}

