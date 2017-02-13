#include <iostream>
#include "AiAgent.hpp"

using namespace JTTW;

AiAgent::AiAgent(Character *controlledCharacter) :
_controlledCharacter(controlledCharacter), _currentBehavior(&AiAgent::stationaryBehavior) {}

void AiAgent::setMap() {

}

void AiAgent::cedeToPlayer(AiAgent *previousPlayer) {
    // clean up any movements from the AI.
    if (_currentBehavior == &AiAgent::stationaryBehavior) {
        // Need to transfer velocity.
        //_controlledCharacter->body->resetForces();
        previousPlayer->_controlledCharacter->transferVelocity(_controlledCharacter);
    } else if (_currentBehavior == &AiAgent::followBehavior) {
        // No need to transfer velocity.
        _controlledCharacter->body->resetForces();
        previousPlayer->_controlledCharacter->transferVelocity(_controlledCharacter);
    }
    previousPlayer->_controlledCharacter->currentCrown->setVisible(false);
    //cocos2d::Vec2 theirOffset = previousPlayer->getPlayerPosOffset();
    previousPlayer->setPlayerPosOffset(previousPlayer->_controlledCharacter->getPosition() - this->_controlledCharacter->getPosition());
    //this->setPlayerPosOffset(theirOffset);
    this->_controlledCharacter->currentCrown->setVisible(true);
}

void AiAgent::retakeFromPlayer(AiAgent *nextPlayer) {
   
}

void AiAgent::adjustOffset(Character *player) {
    this->setPlayerPosOffset(this->_controlledCharacter->getPosition() - player->getPosition());
}

void AiAgent::plan(std::vector<Character *> otherCharacters, cocos2d::EventKeyboard::KeyCode code, bool pressed) {
    if (pressed) {
        changeBehavior(_controlledCharacter, code);
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

    
cocos2d::Vec2 AiAgent::getPlayerPosOffset() const {
    return cocos2d::Vec2(_playerPosOffset);
}

void AiAgent::setPlayerPosOffset(cocos2d::Vec2 playerPosOffset) {
    _playerPosOffset = playerPosOffset;
}


void AiAgent::plan(Character *player, std::vector<Character *> otherCharacters) {
    if (player == _controlledCharacter) {
        // ERROR!!!!
        std::cout << "ERROR: calling wrong function for ai controlled character" << std::endl;
        return;
    }
    (this->*_currentBehavior)(player, otherCharacters);
}

void AiAgent::changeBehavior(Character *player, cocos2d::EventKeyboard::KeyCode code) {
    if (code == cocos2d::EventKeyboard::KeyCode::KEY_A) { // 'A' is becoming a toggle.
        if (_currentBehavior == &AiAgent::stationaryBehavior) {
            _currentBehavior = &AiAgent::followBehavior;
            _playerPosOffset = _controlledCharacter->getPosition() - player->getPosition();
            bool wasOn = _controlledCharacter->currentCrown->isVisible();
            _controlledCharacter->currentCrown->setVisible(false);
            _controlledCharacter->currentCrown = _controlledCharacter->followcrown;
            _controlledCharacter->currentCrown->setVisible(wasOn);
        } else if (_currentBehavior == &AiAgent::followBehavior) {
            _currentBehavior = &AiAgent::stationaryBehavior;
              bool wasOn = _controlledCharacter->currentCrown->isVisible();
            _controlledCharacter->currentCrown->setVisible(false);
            _controlledCharacter->currentCrown = _controlledCharacter->alonecrown;
            _controlledCharacter->currentCrown->setVisible(wasOn);
        }
    }
}

/*
 * A simple force control system that attempts to steer towards the desired position
 * (set by the current behavior).
 */
void AiAgent::executeControl(float delta) {
    // Do the control stuff.
    cocos2d::Vec2 errorPosition = desiredPosition - _controlledCharacter->getPosition();
    cocos2d::Vec2 errorVelocity = desiredVel - _controlledCharacter->body->getVelocity();
    
    cocos2d::Vec2 fprime = errorPosition * kp + errorVelocity * kv;
    
    _controlledCharacter->applyForceRight(fprime.x);
    if (errorPosition.y * kp > 50.0) {
        _controlledCharacter->jump();
    }
}
