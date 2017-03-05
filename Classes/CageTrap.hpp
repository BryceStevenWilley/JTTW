#ifndef CageTrap_hpp
#define CageTrap_hpp

#include "Trap.hpp"
#include <iostream>

namespace JTTW {

class CageTrap : public Trap {
public:
    CageTrap(std::string imageName, cocos2d::Vec2 center, cocos2d::PhysicsMaterial material, 
            cocos2d::Size trapSize, cocos2d::Size imageSize, double wallThickness, double offset);
    
    /** Returns true if trap was triggered. */
    virtual bool triggerTrap(cocos2d::Vec2 characterCenter, cocos2d::Size characterSize);
    
private:
    cocos2d::PhysicsBody *body;
    double minX;
    double maxX;
    double minY;
};

}

#endif /* CageTrap_hpp */
