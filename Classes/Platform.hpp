#ifndef Platform_hpp
#define Platform_hpp

#include "Viewpoint.hpp"

namespace JTTW {
    
// Forward declaration of Viewpoint.
class Viewpoint;
    
class Platform {
    cocos2d::Sprite *image; // to access the physics body, use .getComponent("PhysicsBody");

public:
    Platform();
    Platform(std::string &fileName, cocos2d::Vec2 center, cocos2d::Size imageSize, cocos2d::Vec2 box);
    
    Platform(std::string &fileName, cocos2d::Vec2 center, cocos2d::Size imageSize, std::vector<cocos2d::Vec2> points);

    cocos2d::Sprite *getImage() { return image; }
};
}
#endif /* Platform_hpp */
