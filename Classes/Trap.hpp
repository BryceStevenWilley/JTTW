#ifndef Trap_hpp
#define Trap_hpp

#include "cocos2d.h"
#include <iostream>

namespace JTTW {
class Trap : public cocos2d::Sprite {
public:
    Trap(std::string imageName, cocos2d::Size imageSize, cocos2d::Size trapSize, cocos2d::Vec2 center) :
            cocos2d::Sprite() {
        this->initWithFile(imageName);
        this->setPosition(center);
        this->setContentSize(imageSize);
    }

    /** Returns true if trap was triggered. */
    virtual bool triggerTrap(cocos2d::Vec2 characterCenter, cocos2d::Size characterSize) = 0;
};
}

#endif /* Trap_hpp */