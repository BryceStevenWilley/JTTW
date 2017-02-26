#ifndef Trap_hpp
#define Trap_hpp

#include <iostream>

namespace JTTW {

class Trap : public cocos2d::Sprite {
public:
    Trap(std::string imageName, cocos2d::Vec2 center, cocos2d::PhysicsMaterial material, cocos2d::Size trapSize, cocos2d::Size imageSize, double wallThickness, double offset);
    
    void triggerIfUnder(cocos2d::Vec2 characterCenter);
    
private:
    cocos2d::PhysicsBody *body;
    double minX;
    double maxX;
    double minY;
};

}

#endif /* Trap_hpp */
