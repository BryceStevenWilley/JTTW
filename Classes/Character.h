#ifndef Character_h
#define Character_h

#include "cocos2d.h"
#include "Platform.hpp"
#include "Rectangle.hpp"
#include "MoveableObject.hpp"
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
class Character : public MoveableObject {
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
    Character(const std::string artFileName, JTTW::Rectangle dimensions, cocos2d::Vec2 maxVelocities, double mass, double gravity);
    ~Character();
    
    // Controls for moving the character around.
    // Call these instead of manually changing the velocities member to ensure
    // comprehensible movement.
    virtual void accelerateLeft(float deltaVel);
    virtual void accelerateRight(float deltaVel);
    virtual void stop();
    void jump(float percent);
    void transferVelocity(Character *reciever);
    
    // Convience/ readable functions for checking velocities.
    bool justJumped() const;
    
    // Getters for velocity, mass, and state.
    double getMass() const;
    const State getCurrentState() const;
    
    /**
     * Moves the character over a time period, deltaTime, while avoiding the platforms.
     */
    void move(float deltaTime, std::vector<GameObject *> platforms, bool debugOn);
    
    std::string characterName;

    spine::SkeletonAnimation *ani;
    
private:
    void updateAnimation();
    void updatePosition(double centerX, double centerY);
    
    State currentState = State::STANDING;
    
    // The mass of the character.
    double _mass;
    
    double _gravity;
    
    cocos2d::Vec2 _spawnPosition;
};
} // JTTW

#endif /* Character_h */
