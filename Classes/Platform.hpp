#ifndef Platform_hpp
#define Platform_hpp

#include "Viewpoint.hpp"
#include "Rectangle.hpp"
#include "GameObject.hpp"

namespace JTTW {
    
// Forward declaration of Viewpoint.
class Viewpoint;
    
class Platform : public GameObject {
    cocos2d::Sprite *image;

public:
    Platform();
    Platform(std::string &fileName, cocos2d::Vec2 center, cocos2d::Size imageSize, cocos2d::Vec2 box);
    Platform(std::string fileName, cocos2d::Vec2 centerMeters, cocos2d::Size imageSizeMeters, cocos2d::Vec2 boxMeters, Viewpoint vp);
    
    JTTW::Rectangle getCollisionBounds() { return dimensions; }
    cocos2d::Sprite *getImage() { return image; }
};
}
#endif /* Platform_hpp */
