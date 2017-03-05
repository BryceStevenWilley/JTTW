#ifndef DisappearingPlatform_hpp
#define DisappearingPlatform_hpp

namespace JTTW {
class DisappearingPlatform : public Trap {
    DisappearingPlatform(std::string imageName, cocos2d::Vec2 center, cocos2d::PhysicsMaterial material, 
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

#endif // DisappearingPlatform_hpp