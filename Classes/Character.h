#ifndef Character_h
#define Character_h

#include "cocos2d.h"
#include "Platform.hpp"
#include "Rectangle.hpp"
#include <spine/spine-cocos2dx.h>

namespace JTTW {
    
/**
 * The possible actions that a character can do. Should correspond 1:1
 * to methods in the character class.
 */
enum Action {
    STOP,
    MOVE_LEFT,
    MOVE_RIGHT,
    JUMP
};

// Forward declaration of Platform.
class Platform;
    
/**
 * @brief Class for interactive characters.
 *
 * Position is contained in the sprite, and is accessed through sprite->getPosition()/sprite->setPosition(<new Vec2>).
 * Velocity is a percentage that goes from -1.0 to 1.0 contained in the velocity member, but should only be
 * changed through accelerateLeft and accelerateRight to avoid non-sensical movement.
 */
class Character {
public:
    enum State {
        STANDING,
        MID_AIR
    };

    /**
     * Simple constructor.
     * Example use:
     * // Makes a 40px x 40px character, with maximum horizontal velocity of 200px/second
     * // and vertical velocity of 100px/second, and gravity of 100px/second^2.
     * Character c("spineboy", JTTW::Rectangle(0, 20, 40, 40), cocos2d::Vec2(200, 100), 100);
     */
    Character(const std::string artFileName, JTTW::Rectangle dimensions, cocos2d::Vec2 maxVelocities, double gravity);
    ~Character();
    
    // Controls for moving the character around.
    // Call these instead of manually changing the velocities member to ensure
    // comprehensible movement.
    void accelerateLeft(float deltaVel);
    void accelerateRight(float deltaVel);
    void stop();
    void jump(float percent);
    void transferVelocity(Character *reciever);
    
    // Convience/ readable functions for checking velocities.
    bool isMovingLeft() const;
    bool isMovingRight() const;
    bool justJumped() const;
    
    // Getters for velocity and state.
    double getXVelocity() const;
    double getYVelocity() const;
    const State getCurrentState() const;
    
    /**
     * Moves the character over a time period, deltaTime, while avoiding the platforms.
     */
    void move(float deltaTime, std::vector<Platform> platforms, bool debugOn);

    spine::SkeletonAnimation *ani;
    
    // A box that encompasses the character.
    JTTW::Rectangle dimensions;

    std::string characterName;
    
    cocos2d::DrawNode *collisionBoxNode;
    
private:
    void updateAnimation();
    void updatePosition(double centerX, double centerY);
    void drawCollisionBox();
    
    State currentState = State::STANDING;
    
    // Vector of x and y velocities, normalized so that 1 moves right/up at max speed, -1 moves left/down at max speed.
    cocos2d::Vec2 velocities = cocos2d::Vec2(0.0, 0.0);
    
    // The left and right forces on the character. Forces include movement actions,
    // collisions with platforms, wind, etc.
    double forceXLeft = 0.0;
    double forceXRight = 0.0;
    
    // The max speed that the character should move laterally in pixels per second.
    cocos2d::Vec2 _maxVelocities;

    double _gravity;
};
} // JTTW

#endif /* Character_h */
