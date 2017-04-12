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

cocos2d::Vec2 Viewpoint::getNewLevelVec(cocos2d::Node *player, cocos2d::Size _screenDims, double _scale) {
    // So, we know the level location, the width of the screen, and the player location.
    // When level->position.x == 0, then the center of the screen is at width/2.0.
    // width/2.0 = level->position.x + player->getPosition().x;
    cocos2d::Vec2 centerPoint = player->getPosition();
    
    cocos2d::Size scaledDims = _screenDims * (1.0 / _scale);
    
    // Make sure that level view doesn't go out of limits.
    float xmin = centerPoint.x - scaledDims.width / 2.0;
    float xmax = centerPoint.x + scaledDims.width / 2.0;
    float ymin = centerPoint.y - scaledDims.height / 2.0;
    float ymax = centerPoint.y + scaledDims.height / 2.0;
    
    if (xmax > maxCornerLimit.x) {
        // Shift left.
        centerPoint.x = maxCornerLimit.x - scaledDims.width / 2.0;
    }
    
    if (xmin < minCornerLimit.x) {
        // Shift right.
        centerPoint.x = minCornerLimit.x + scaledDims.width / 2.0;
    }
    
    if (ymax > maxCornerLimit.y) {
        // Shift down.
        centerPoint.y = maxCornerLimit.y - scaledDims.height / 2.0;
    }
    
    if (ymin < minCornerLimit.y) {
        // Shift up.
        centerPoint.y = minCornerLimit.y + scaledDims.height / 2.0;
    }
    
    float newLevelX = (_screenDims.width/2.0 - centerPoint.x) * _scale;
    float newLevelY = (_screenDims.height/2.0 - centerPoint.y) * _scale;

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

void Viewpoint::setLimits(cocos2d::Vec2 min, cocos2d::Vec2 max) {
    minCornerLimit = min;
    maxCornerLimit = max;
    
    std::cout << "New max and min camera limits: " << min.x << ", " << min.y << " to " <<  max.x << ", " << max.y <<std::endl;
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
        cocos2d::Vec2 newPos = getNewLevelVec(player, _screenDims, _scale);
        _level->setPosition(newPos);
       // background->setPosition(newPos);
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
        cocos2d::Vec2 newPos = getNewLevelVec(body, _screenDims, _scale);
        _level->setPosition(newPos);
       // background->setPosition(newPos);
    } else {
        //_level->runAction(cocos2d::MoveBy::create(delta, cocos2d::Vec2(-body->getVelocity().x * delta, -body->getVelocity().y * delta)));
    }
}
