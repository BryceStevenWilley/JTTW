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

    Platform(std::string &fileName, cocos2d::Vec2 center, cocos2d::Size imageSize, cocos2d::Vec2 box, bool climeable, bool collidable);
    
protected:
    cocos2d::PhysicsBody *body;
};
}
#endif /* Platform_hpp */
