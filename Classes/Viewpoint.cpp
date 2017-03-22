//
//  Viewpoint.cpp
//  JTTW
//
//  Created by Bryce Willey on 1/16/17.
//
//

#include "Viewpoint.hpp"

using namespace JTTW;

Viewpoint::Viewpoint(cocos2d::Size dims, double metersPerPixel) :
_screenDims(dims), _metersPerPixel(metersPerPixel), _level(nullptr) {}

cocos2d::Vec2 getNewLevelVec(cocos2d::Node *player, cocos2d::Size _screenDims, double _scale) {
    // So, we know the level location, the width of the screen, and the player location.
    // When level->position.x == 0, then the center of the screen is at width/2.0.
    // width/2.0 = level->position.x + player->getPosition().x;
    float newLevelX = (_screenDims.width/2.0 - player->getPosition().x) * _scale;
    float newLevelY = (_screenDims.height/2.0 - player->getPosition().y) * _scale;

    return cocos2d::Vec2(newLevelX, newLevelY);
}

void Viewpoint::setLayer(cocos2d::Layer *level) {
    _level = level;
}

void Viewpoint::setRatio(double mToPixel) {
    _metersPerPixel = 1.0/mToPixel;
}

void Viewpoint::setScale(double screenOverIdeal) {
    _scale = screenOverIdeal;
}

int Viewpoint::metersToPixels(double meters) const {
    return meters / _metersPerPixel;
}

cocos2d::Vec2 Viewpoint::metersToPixels(cocos2d::Vec2 vecMeters) const {
    return cocos2d::Vec2(metersToPixels(vecMeters.x), metersToPixels(vecMeters.y));
}

double Viewpoint::pixelsToMeters(int pixels) const {
    return pixels * _metersPerPixel;
}

void Viewpoint::panToCharacter(cocos2d::Node* player) {
    if (_level == nullptr) {
        std::cout << "_level was never set for Viewpoint!" << std::endl;
        std::exit(0);
    }
    auto action = cocos2d::MoveTo::create(.5, getNewLevelVec(player, _screenDims, _scale));
    
    auto callback = cocos2d::CallFuncN::create([&](cocos2d::Node* sender)
    {
        _isPanning = false;
    });

    _isPanning = true;
    _level->runAction(cocos2d::Sequence::create(action, callback, nullptr));
}

void Viewpoint::followCharacter(Character *player, float delta) {
    if (_level == nullptr) {
        std::cout << "_level was never set for Viewpoint!" << std::endl;
        std::exit(0);
    }
    if (_isPanning == false) {
        _level->setPosition(getNewLevelVec(player, _screenDims, _scale));
    } else {
        _level->runAction(cocos2d::MoveBy::create(delta, cocos2d::Vec2(-player->body->getVelocity().x * delta, -player->body->getVelocity().y * delta)));
    }
}

void Viewpoint::followCharacter(cocos2d::Node *body, float delta) {
    if (_level == nullptr) {
        std::cout << "_level was never set for Viewpoint!" << std::endl;
        std::exit(0);
    }
    if (_isPanning == false) {
        _level->setPosition(getNewLevelVec(body, _screenDims, _scale));
    } else {
        //_level->runAction(cocos2d::MoveBy::create(delta, cocos2d::Vec2(-body->getVelocity().x * delta, -body->getVelocity().y * delta)));
    }
}
