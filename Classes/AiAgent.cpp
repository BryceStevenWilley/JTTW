#include <iostream>
#include "AiAgent.hpp"
#include "Resolution.hpp"
#include "Monkey.hpp"

using namespace JTTW;

const float IMPULSE_AMOUNT = ideal2Res(280.0);
const float ERROR_UP = ideal2Res(20.0);
const float ERROR_DOWN = ideal2Res(5.0);
const float MAX_HORI_VEL = ideal2Res(400);

AiAgent::AiAgent(Character *controlledCharacter) :
_controlledCharacter(controlledCharacter), _currentBehavior(&AiAgent::noResistenceBehavior) {}

void AiAgent::switchUserToHereFrom(AiAgent *previousUser) {
    // clean up any movements from the AI.
    if (_currentBehavior == &AiAgent::noResistenceBehavior) {
        // Need to transfer velocity.
        previousUser->_controlledCharacter->transferVelocity(_controlledCharacter);
    } else if (_currentBehavior == &AiAgent::followBehavior || _currentBehavior == &AiAgent::goToPointBehavior) {
        // Need to transfer velocity, and reset the control forces that were previously acting there.
        _controlledCharacter->body->resetForces();
        previousUser->_controlledCharacter->transferVelocity(_controlledCharacter);
    }
    previousUser->_controlledCharacter->toggleToAI();
    previousUser->setPlayerPosOffset(previousUser->_controlledCharacter->getPosition() - this->_controlledCharacter->getPosition());
    previousUser->_controlledCharacter->currentCrown->setVisible(false);
    this->_controlledCharacter->toggleToPlayer();
    this->_controlledCharacter->currentCrown->setVisible(true);
}

void AiAgent::adjustOffset(Character *player) {
    this->setPlayerPosOffset(this->_controlledCharacter->getPosition() - player->getPosition());
}

void AiAgent::plan(std::vector<Character *> otherCharacters, cocos2d::EventKeyboard::KeyCode code, bool pressed) {
    changeBehavior(_controlledCharacter, code, pressed);
    
    // All code to control goes here.
    using KeyCode = cocos2d::EventKeyboard::KeyCode;
    
    switch(code) {
        case KeyCode::KEY_LEFT_ARROW:
            if (pressed) {
                _controlledCharacter->impulseLeft(IMPULSE_AMOUNT);
            } else { // released
                _controlledCharacter->impulseLeft(-IMPULSE_AMOUNT);
            }
            break;
        case KeyCode::KEY_RIGHT_ARROW:
            if (pressed) {
                _controlledCharacter->impulseRight(IMPULSE_AMOUNT);
            } else {
                _controlledCharacter->impulseRight(-IMPULSE_AMOUNT);
            }
            break;
        case KeyCode::KEY_SPACE:
            if (pressed) {
                _controlledCharacter->initJump();
            } else { // released
                _controlledCharacter->stopJump();
            }
            break;
        case KeyCode::KEY_S:
            //if (pressed) {
            //    // Do animation of character calling for everyone else.
            //    _controlledCharacter->callHey();
           // }
            break;
        default:
            // Try the charecter's special controls.
            // Commented out because we want these to always be used, so they go in changeBehavior.
            //_controlledCharacter->characterSpecial(code, pressed);
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
    double dist = player->getPosition().distance(_controlledCharacter->getPosition());
    if (dist < ideal2Res(850)) {
         (this->*_currentBehavior)(player, otherCharacters);
    } else {
        this->noResistenceBehavior(player, otherCharacters);
    }
}

    
void AiAgent::equipFollowBehavior(Character *player) {
    if (player == _controlledCharacter) {
        // Do animation of character calling for everyone else.
        _controlledCharacter->callHey();
    }
    _currentBehavior = &AiAgent::followBehavior;
    if (player->characterName == "Monkey" && ((Monkey *)player)->getMonkeyState() == Monkey::State::SWINGING) {
        _playerPosOffset = cocos2d::Vec2(0, 0) + cocos2d::Vec2(ideal2Res((rand() % 120)) - ideal2Res(60), 0);
    } else {
        _playerPosOffset = cocos2d::Vec2(ideal2Res(-100), 0) + cocos2d::Vec2(ideal2Res((rand() % 120)) - ideal2Res(60), 0);
    }
    bool wasOn = _controlledCharacter->currentCrown->isVisible();
    _controlledCharacter->currentCrown->setVisible(false);
    _controlledCharacter->currentCrown = _controlledCharacter->followcrown;
    _controlledCharacter->currentCrown->setVisible(wasOn);
}

void AiAgent::equipNoResistenceBehavior(Character *player) {
    _currentBehavior = &AiAgent::noResistenceBehavior;
    bool wasOn = _controlledCharacter->currentCrown->isVisible();
    _controlledCharacter->currentCrown->setVisible(false);
    _controlledCharacter->currentCrown = _controlledCharacter->alonecrown;
    _controlledCharacter->currentCrown->setVisible(wasOn);
}

void AiAgent::changeBehavior(Character *player, cocos2d::EventKeyboard::KeyCode code, bool pressed) {
    if (pressed && code == cocos2d::EventKeyboard::KeyCode::KEY_A) { // 'A' is becoming a toggle.
        if (_currentBehavior == &AiAgent::noResistenceBehavior || _currentBehavior == &AiAgent::goToPointBehavior) {
            equipFollowBehavior(player);
        } else if (pressed && _currentBehavior == &AiAgent::followBehavior) {
            equipNoResistenceBehavior(player);
        }
    } else {
        // Try their specials.
        if (code == cocos2d::EventKeyboard::KeyCode::KEY_D ||
            (player->getCurrentState() == Character::State::HANGING &&
             _controlledCharacter->characterName == "Monkey") ||
             player->characterName == "Monkey") {
            _controlledCharacter->characterSpecial(code, pressed);
        }
    }
}

/*
 * A simple force control system that attempts to steer towards the desired position
 * (set by the current behavior).
 */
void AiAgent::executeControl(float delta) {
    if (!_controlledCharacter->shouldBeControlled() || _currentBehavior == &AiAgent::noResistenceBehavior) {
        std::cout << "Not controlling " << _controlledCharacter->characterName << std::endl;
        return;
    }
    // Do the control stuff.
    cocos2d::Vec2 errorPosition = desiredPosition - _controlledCharacter->getPosition();
    cocos2d::Vec2 errorVelocity = desiredVel - _controlledCharacter->body->getVelocity();
    
    cocos2d::Vec2 fprime = errorPosition * kp + errorVelocity * kv;
    
    // Cap the force applied.
    if (fprime.x > 7000) {
        fprime.x = 7000;
    }
    
    if (_controlledCharacter->getCurrentState() == Character::State::MID_AIR &&
        _controlledCharacter->body->getVelocity().getLengthSq() < 1 &&
        std::abs(fprime.x) > 1000) {
        std::cout << _controlledCharacter->characterName << " is stuck in mid air against a wall." << std::endl;
        // TODO: Stop, jump, get to peak, and continue? 
    }
    
    _controlledCharacter->applyForceRight(fprime.x);
    
    // Cap the character at the maximum horizontal velocity (negative or positive).
    if (std::abs(_controlledCharacter->body->getVelocity().x) > MAX_HORI_VEL) {
        int sign = 1;
        if (_controlledCharacter->body->getVelocity().x < 0) {
            sign = -1;
        }
        _controlledCharacter->body->setVelocity(cocos2d::Vec2(MAX_HORI_VEL * sign, _controlledCharacter->body->getVelocity().y));
    }
    if (errorPosition.y > ERROR_UP) {
        _controlledCharacter->initJump();
    } else if (errorPosition.y < ERROR_DOWN) {
        _controlledCharacter->stopJump();
    }
}
