//
//  Vine.hpp
//  JTTW
//
//  Created by Bryce Willey on 2/20/17.
//
//

#ifndef Vine_hpp
#define Vine_hpp

#include <iostream>

namespace JTTW {
class Vine : public cocos2d::Sprite {
private:
    //cocos2d::Sprite *image; // to get physics body use .getComponent("PhysicsBody");
    cocos2d::PhysicsBody *body;
    cocos2d::Vec2 rotCenter;
    
public:
    Vine(std::string fileName, cocos2d::Vec2 rotCenter, double width, double length);

    cocos2d::PhysicsBody *getBody();
    cocos2d::Vec2 getRotationCenter();
    
    void move();
};
}

#endif /* Vine_hpp */
