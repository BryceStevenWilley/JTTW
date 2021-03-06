#ifndef Boulder_hpp
#define Boulder_hpp

#include "cocos2d.h"
#include <iostream>

namespace JTTW {

class Boulder : public cocos2d::Sprite {
public:
    Boulder(double radius, std::string imageName, cocos2d::Vec2 center, double mass, cocos2d::Size imgSize);
    Boulder(std::vector<cocos2d::Vec2> points, std::string imageName, cocos2d::Vec2 center, double mass, cocos2d::Size imgSize);
    
    cocos2d::PhysicsBody *getBody();
    
private:
    cocos2d::PhysicsBody *body;
};

}
#endif /* Boulder_hpp */
