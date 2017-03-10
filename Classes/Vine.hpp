#ifndef Vine_hpp
#define Vine_hpp

#include "cocos2d.h"
#include <iostream>

namespace JTTW {
class Vine : public cocos2d::Sprite {
private:
    cocos2d::PhysicsBody *body;
    cocos2d::Vec2 rotCenter;
    double _length;
    
public:
    Vine(std::string fileName, cocos2d::Vec2 rotCenter, double width, double length, double startingAngVel);

    cocos2d::PhysicsBody *getBody();
    cocos2d::Vec2 getRotationCenter();
    double getLength() const;
};
}

#endif /* Vine_hpp */
