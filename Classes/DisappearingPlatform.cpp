#include <iostream>
#include "DisappearingPlatform.hpp"

using namespace JTTW;

DisappearingPlatform::DisappearingPlatform(std::string &fileName, cocos2d::Vec2 center,
                                           cocos2d::Size imageSize, std::vector<cocos2d::Vec2> points,
                                           bool climeable, bool collidable) 
        :  Platform(fileName, center, imageSize, points, std::vector<double>(), std::vector<bool>(), climeable, collidable) {
    _maxX = center.x + imageSize.width / 2.0;
    _minX = center.x - imageSize.width / 2.0;
    _maxY = center.y + imageSize.height / 2.0 + 30;
    _minY = center.y - imageSize.width / 2.0;
}

bool DisappearingPlatform::triggerTrap(cocos2d::Vec2 characterCenter, cocos2d::Size characterSize) {
    if (characterCenter.x > _minX && characterCenter.x < _maxX &&
        characterCenter.y - characterSize.height / 2.0 > _minY &&
        characterCenter.y - characterSize.height / 2.0 < _maxY) {
        this->body->removeFromWorld();
        this->removeComponent(this->body);
        this->runAction(cocos2d::FadeOut::create(3.0));
        return true;
    }
    return false;
}

Action DisappearingPlatform::characterReaction() const {
    return Action::FALL;
}
