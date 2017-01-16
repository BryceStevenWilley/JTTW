#ifndef Character_h
#define Character_h

#include "cocos2d.h"

/**
 * @brief Class for interactive characters
 */
namespace JTTW {
class Character {
public:
    enum State {
        STANDING,
        MID_AIR
    };

    Character(const std::string artFileName, cocos2d::Vec2 dimensions, cocos2d::Vec2 maxVelocities);
    ~Character();
    
    /**
     * Controls for moving the character around.
     * Call these instead of manually changing the velocities member to ensure
     * Comprehensible movement.
     */
    void accelerateLeft(float deltaVel);
    void accelerateRight(float deltaVel);
    void stop();
    void jump(float percent);
    void transferVelocity(Character *reciever);
    
    bool isMovingLeft() const;
    bool isMovingRight() const;
    bool justJumped() const;
    
    const State getCurrentState() const;
    
    /**
     * Moves the character left and right.
     *
     *  direction is either 1 or -1, to make the character move left or right respectively.
     */
    void move(float deltaTime);
    
    // TODO: integrate Mei's art with this.
    cocos2d::Sprite *sprite;
    
    // A box that encompasses the character.
    cocos2d::Vec2 dimensions;
    
private:
    State currentState = State::STANDING;
    
    // Vector of x and y velocities, normalized so that 1 moves right/up at max speed, -1 moves left/down at max speed.
    cocos2d::Vec2 velocities = cocos2d::Vec2(0.0, 0.0);
    
    // The max speed that the character should move laterally in pixels per second.
    cocos2d::Vec2 _maxVelocities;

};
}; // JTTW

#endif /* Character_h */
