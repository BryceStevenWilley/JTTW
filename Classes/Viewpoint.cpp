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

double Viewpoint::pixelsToMeters(int pixels) const {
    return pixels * _metersPerPixel;
}

void Viewpoint::panToCharacter(Character* player) {
    // So, we know the level location, the width of the screen, and the player location.
    // When level->position.x == 0, then the center of the screen is at width/2.0.
    // width/2.0 = level->position.x + player->ani->position.x;
    float newLevelX = _screenDims.width/2.0 - player->ani->getPosition().x;
    auto action = cocos2d::MoveTo::create(1.0, cocos2d::Vec2(newLevelX, _level->getPosition().y));
    _level->runAction(action);
}

void Viewpoint::followCharacter(Character *player) {
    float newLevelX = _screenDims.width / 2.0 - player->ani->getPosition().x;
    _level->setPosition(cocos2d::Vec2(newLevelX, _level->getPosition().y));
}
