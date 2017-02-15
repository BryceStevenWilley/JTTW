#ifndef Platform_hpp
#define Platform_hpp

#include "Viewpoint.hpp"

namespace JTTW {
    
// Forward declaration of Viewpoint.
class Viewpoint;
    
const int CLIMBEABLE_TAG = 11;

class Platform {
    cocos2d::Sprite *image; // to access the physics body, use .getComponent("PhysicsBody");
    
public:
    Platform();
    Platform(std::string &fileName, cocos2d::Vec2 center, cocos2d::Size imageSize, cocos2d::Vec2 box, bool climeable);

    cocos2d::Sprite *getImage() { return image; }
};
}
#endif /* Platform_hpp */
