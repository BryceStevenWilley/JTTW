#ifndef DisappearingPlatform_hpp
#define DisappearingPlatform_hpp

#include "Platform.hpp"
#include "Trap.hpp"

namespace JTTW {
class DisappearingPlatform : public Trap, public Platform {
public:
    DisappearingPlatform(std::string &fileName, cocos2d::Vec2 center, cocos2d::Size imageSize, cocos2d::Vec2 box, bool climeable, bool collidable);

    /** Returns true if trap was triggered. */
    virtual bool triggerTrap(cocos2d::Vec2 characterCenter, cocos2d::Size characterSize);
    virtual Action characterReaction() const;
    
private:
    double _maxX;
    double _minX;
    double _maxY;
    double _minY;
};
}

#endif // DisappearingPlatform_hpp
