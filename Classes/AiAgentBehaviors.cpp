// More implementations for the AiAgent class.
//
// This is seperate from AiAgent.cpp because this will eventually
// get to be very large, containing all possible behaviors for all
// characters.

#include "AiAgent.hpp"

using namespace JTTW;

/**
 * Sets the desired position/trajectory to be at an offset from the main player.
 * Completely dumb, will jump more than it needs to, etc.
 */
void AiAgent::followBehavior(Character *player, std::vector<Character *> otherCharacters) {
    // Set the desired velocity and position, as taken from input from the player character.
    desiredVel = player->body->getVelocity() * _controlledCharacter->_impulseScale * player->_impulseScale;
    desiredPosition = player->getPosition() + _playerPosOffset;
}

/**
 * Sets the desired position to be exactly where they are.
 * Don't move an inch.
 */
void AiAgent::stationaryBehavior(Character *player, std::vector<Character *> otherCharacters) {
    desiredVel = cocos2d::Vec2::ZERO;
    desiredPosition = _controlledCharacter->getPosition();
}

/**
 * Will go to the absolute world point, goToPoint, which is set when the player presses 'Q' and
 * companions are not following.
 */
void AiAgent::goToPointBehavior(Character *player, std::vector<Character *> otherCharacters) {
    desiredVel = cocos2d::Vec2::ZERO;
    desiredPosition = goToPoint;
    
    if (_controlledCharacter->getPosition() == desiredPosition) {
        // If you've reached the desired point, then stay there and switch to stationary behavior.
        std::cout << _controlledCharacter->characterName << " reached goto and is switching to stationary behavior." << std::endl;
        _currentBehavior = &AiAgent::stationaryBehavior;
    }
}
