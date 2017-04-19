#ifndef Dragon_hpp
#define Dragon_hpp

#include <iostream>
#include <spine/spine-cocos2dx.h>
#include "SceneObject.hpp"
#include "Quicksand.hpp"
#include "cocos2d.h"

namespace JTTW {

class Dragon : public spine::SkeletonAnimation {
public:
    Dragon(cocos2d::Vec2 startPosition, cocos2d::Size dimensions);
    
    void move(cocos2d::Vec2 vel);
    
private:
    cocos2d::PhysicsBody *body;
};

}

#endif /* Dragon_hpp */
