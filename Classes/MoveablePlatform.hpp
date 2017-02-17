#ifndef MoveablePlatform_hpp
#define MoveablePlatform_hpp

#include "Viewpoint.hpp"
#include "Platform.hpp"

namespace JTTW {

// Forward declaration of Viewpoint.
class Viewpoint;


const int MOVEABLE_TAG = 12;

class MoveablePlatform : public Platform {
public:
    enum State {
        TOWARDS_A,
        TOWARDS_B
    };
    
    cocos2d::Sprite *image;
    cocos2d::PhysicsBody *body;

    MoveablePlatform(std::string &fileName, cocos2d::Vec2 centerA, cocos2d::Vec2 centerB, cocos2d::Size imageSize, cocos2d::Vec2 box, double maxVelocity);
    MoveablePlatform(std::string &fileName, cocos2d::Vec2 centerAm, cocos2d::Vec2 centerBm, cocos2d::Size imageSizeM, cocos2d::Vec2 boxM, double maxVelocity, Viewpoint vp);
    
    // NOTE: we currently assume that the platform should hit no other platform when moving, at all.
    void move(float deltaTime, bool debugOn);
    
private:
    State currentState = State::TOWARDS_B;
    
    cocos2d::Vec2 _centerA;
    cocos2d::Vec2 _centerB;
    
    cocos2d::Vec2 _velocities;
};
}

#endif /* MoveablePlatform_hpp */
