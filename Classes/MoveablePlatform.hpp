#ifndef MoveablePlatform_hpp
#define MoveablePlatform_hpp

#include "MoveableObject.hpp"

namespace JTTW {

// Forward declaration of Viewpoint.
class Viewpoint;

class MoveablePlatform : MoveableObject {
public:
    enum State {
        TOWARDS_A,
        TOWARDS_B
    };
    
    cocos2d::Sprite *image;
    
    MoveablePlatform();
    MoveablePlatform(std::string &fileName, cocos2d::Vec2 centerA, cocos2d::Vec2 centerB, cocos2d::Size imageSize, cocos2d::Vec2 box);
    MoveablePlatform(std::string &fileName, cocos2d::Vec2 centerAm, cocos2d::Vec2 centerBm, cocos2d::Size imageSizeM, cocos2d::Vec2 boxM, Viewpoint vp);
    
};
}

#endif /* MoveablePlatform_hpp */
