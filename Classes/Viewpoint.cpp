//
//  Viewpoint.cpp
//  JTTW
//
//  Created by Bryce Willey on 1/16/17.
//
//

#include "Viewpoint.hpp"

using namespace JTTW;

Viewpoint::Viewpoint(cocos2d::Size dims, double metersPerPixel, cocos2d::Layer *level) :
_screenDims(dims), _metersPerPixel(metersPerPixel), _level(level) {}

int Viewpoint::metersToPixels(double meters) const {
    return meters / _metersPerPixel;
}

cocos2d::Vec2 Viewpoint::metersToPixels(cocos2d::Vec2 vecMeters) const {
    return cocos2d::Vec2(metersToPixels(vecMeters.x), metersToPixels(vecMeters.y));
}

double Viewpoint::pixelsToMeters(int pixels) const {
    return pixels * _metersPerPixel;
}

void Viewpoint::panToCharacter(Character* player) {
    // So, we know the level location, the width of the screen, and the player location.
    // When level->position.x == 0, then the center of the screen is at width/2.0.
    // width/2.0 = level->position.x + player->ani->position.x;
    float newLevelX = _screenDims.width/2.0 - player->getPosition().x;
    float newLevelY = _screenDims.height/2.0 - player->getPosition().y;
    auto action = cocos2d::MoveTo::create(.5, cocos2d::Vec2(newLevelX, newLevelY));
    
    auto callback = cocos2d::CallFuncN::create([&](cocos2d::Node* sender)
    {
        _isPanning = false;
    });

    _isPanning = true;
    _level->runAction(cocos2d::Sequence::create(action, callback, nullptr));
}

void Viewpoint::followCharacter(Character *player, float delta) {
    if (_isPanning == false) {
        float newLevelX = _screenDims.width / 2.0 - player->getPosition().x;
        float newLevelY =  _screenDims.height / 2.0 - player->getPosition().y;
        _level->setPosition(cocos2d::Vec2(newLevelX, newLevelY));
    } else {
        _level->runAction(cocos2d::MoveBy::create(delta, cocos2d::Vec2(-player->body->getVelocity().x * delta, -player->body->getVelocity().y * delta)));
    }
}

void Viewpoint::followCharacter(cocos2d::Node *body, float delta) {
    if (_isPanning == false) {
        float newLevelX = _screenDims.width / 2.0 - body->getPosition().x;
        float newLevelY =  _screenDims.height / 2.0 - body->getPosition().y;
        _level->setPosition(cocos2d::Vec2(newLevelX, newLevelY));
    } else {
        //_level->runAction(cocos2d::MoveBy::create(delta, cocos2d::Vec2(-body->getVelocity().x * delta, -body->getVelocity().y * delta)));
    }
}
