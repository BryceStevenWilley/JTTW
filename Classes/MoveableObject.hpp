#ifndef MoveableObject_hpp
#define MoveableObject_hpp

#include "Rectangle.hpp"
#include "GameObject.hpp"

namespace JTTW {
class MoveableObject : public GameObject {
public:
    MoveableObject(JTTW::Rectangle dimensions, cocos2d::Vec2 maxVelocities);

    void stop();
    virtual void accelerateLeft(float deltaVel);
    virtual void accelerateRight(float deltaVel);
    
    // Convience/ readable functions for checking velocities.
    bool isMovingLeft() const;
    bool isMovingRight() const;
    
    // Getters for velocity, mass, and state.
    double getXVelocity() const;
    double getYVelocity() const;

protected:
    // Vector of x and y velocities, normalized so that 1 moves right/up at max speed, -1 moves left/down at max speed.
    cocos2d::Vec2 velocities = cocos2d::Vec2(0.0, 0.0);
    
    // The max speed that the character should move laterally in pixels per second.
    cocos2d::Vec2 _maxVelocities;
    
    // The left and right forces on the character. Forces include movement actions,
    // collisions with platforms, wind, etc.
    double forceXLeft = 0.0;
    double forceXRight = 0.0;
    
    cocos2d::PhysicsBody *physics;
};
}

#endif /* MoveableObject_hpp */
