//
//  Platform.hpp
//  JTTW
//
//  Created by Bryce Willey on 1/25/17.
//
//

#ifndef Platform_hpp
#define Platform_hpp

#include <stdio.h>
#include "Viewpoint.hpp"
#include "Rectangle.hpp"

namespace JTTW {
    
// Forward declaration of Viewpoint.
class Viewpoint;
    
class Platform {
    cocos2d::Sprite *image;
    JTTW::Rectangle collisionBox; // center of this is the center of the image.
    
public:
    Platform();
    Platform(std::string &fileName, cocos2d::Vec2 center, cocos2d::Size imageSize, cocos2d::Vec2 box);
    Platform(std::string fileName, cocos2d::Vec2 centerMeters, cocos2d::Size imageSizeMeters, cocos2d::Vec2 boxMeters, Viewpoint vp);
    
    JTTW::Rectangle getCollisionBounds() { return collisionBox; }
    cocos2d::Sprite *getImage() { return image; }
};
}
#endif /* Platform_hpp */
