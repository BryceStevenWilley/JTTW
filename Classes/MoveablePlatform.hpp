#ifndef MoveablePlatform_hpp
#define MoveablePlatform_hpp

#include "Viewpoint.hpp"
#include "Platform.hpp"
#include "Moveable.hpp"

namespace JTTW {

// Forward declaration of Viewpoint.
class Viewpoint;

class MoveablePlatform : public Moveable {
public:
    enum State {
        TOWARDS_A,
        TOWARDS_B
    };

    MoveablePlatform(std::string &fileName, cocos2d::Vec2 centerA, cocos2d::Vec2 centerB, cocos2d::Size imageSize, std::vector<cocos2d::Vec2> points, std::vector<double> frictions, double maxVelocity);

    // NOTE: we currently assume that the platform should hit no other platform when moving, at all.
    virtual void move(float deltaTime);
    
private:
    State currentState = State::TOWARDS_B;
    
    double _maxVelocity;
};
}

#endif /* MoveablePlatform_hpp */
