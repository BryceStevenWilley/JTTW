#ifndef MoveablePlatform_hpp
#define MoveablePlatform_hpp

#include "MoveableObject.hpp"
#include "Viewpoint.hpp"

namespace JTTW {

// Forward declaration of Viewpoint.
class Viewpoint;

class MoveablePlatform : public MoveableObject {
public:
    enum State {
        TOWARDS_A,
        TOWARDS_B
    };
    
    cocos2d::Sprite *image;

    MoveablePlatform(std::string &fileName, cocos2d::Vec2 centerA, cocos2d::Vec2 centerB, cocos2d::Size imageSize, cocos2d::Vec2 box, double maxVelocity);
    MoveablePlatform(std::string &fileName, cocos2d::Vec2 centerAm, cocos2d::Vec2 centerBm, cocos2d::Size imageSizeM, cocos2d::Vec2 boxM, double maxVelocity, Viewpoint vp);
    
    // NOTE: we currently assume that the platform should hit no other platform when moving, at all.
    void move(float deltaTime, bool debugOn);
    
private:
    void updatePosition(double centerX, double centerY);
    
    State currentState = State::TOWARDS_B;
    
    cocos2d::Vec2 _centerA;
    cocos2d::Vec2 _centerB;
};
}

#endif /* MoveablePlatform_hpp */
