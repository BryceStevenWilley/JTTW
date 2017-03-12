#ifndef Platform_hpp
#define Platform_hpp

#include "Viewpoint.hpp"

namespace JTTW {
    
// Forward declaration of Viewpoint.
class Viewpoint;
    
class Platform: public cocos2d::Sprite {
public:
    Platform();

    Platform(std::string &fileName, cocos2d::Vec2 center, cocos2d::Size imageSize, std::vector<cocos2d::Vec2> points, bool climeable, bool collidable);

protected:
    cocos2d::PhysicsBody *body;

private:
    const cocos2d::PhysicsMaterial DEFAULT_MATERIAL = cocos2d::PhysicsMaterial(1.0, 0.0, 1.0);
};
}
#endif /* Platform_hpp */
