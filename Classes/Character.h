#ifndef Character_h
#define Character_h

#include "cocos2d.h"
#include <spine/spine-cocos2dx.h>

/**
 * @brief Class for interactive characters.
 *
 * Position is contained in the sprite, and is accessed through sprite->getPosition()/sprite->setPosition(<new Vec2>).
 * Velocity is a percentage that goes from -1.0 to 1.0 contained in the velocity member, but should only be
 * changed through accelerateLeft and accelerateRight to avoid non-sensical movement.
 */
namespace JTTW {
    
struct BadPlatform {
    cocos2d::Sprite *s;
    cocos2d::Rect dimensions;
    
    BadPlatform() :
            s(cocos2d::Sprite::create()),
            dimensions(cocos2d::Rect()) {}
    
    BadPlatform(std::string fileName) :
            s(cocos2d::Sprite::create(fileName)),
            dimensions(cocos2d::Rect()) {
                s->setAnchorPoint(cocos2d::Vec2(0.0, 0.0));
    }
};

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
    
class Character {
public:
    enum State {
        STANDING,
        MID_AIR
    };

    /**
     * Simple constructor.
     * Example use:
     * // Make a 40px x 40px character, with maximum horizontal velocity of 200px/second
     * // and vertical velocity of 100px/second, and gravity of 100px/second^2.
     * Character c("image.png", cocos2d::Vec2(40, 40), cocos2d::Vec2(200, 100), 100);
     */
    Character(const std::string artFileName, cocos2d::Vec2 dimensions, cocos2d::Vec2 maxVelocities, double gravity);
    ~Character();
    
    /**
     * Controls for moving the character around.
     * Call these instead of manually changing the velocities member to ensure
     * comprehensible movement.
     */
    void accelerateLeft(float deltaVel);
    void accelerateRight(float deltaVel);
    void stop();
    void jump(float percent);
    void transferVelocity(Character *reciever);
    
    // Convience/ readable functions for checking velocities.
    bool isMovingLeft() const;
    bool isMovingRight() const;
    bool justJumped() const;
    
    // Getters for velocity.
    double getXVelocity() const;
    double getYVelocity() const;
    
    const State getCurrentState() const;
    
    bool isDirectlyAbove(cocos2d::Rect platform, cocos2d::Vec2 center, cocos2d::Vec2 dims);
    
    /**
     * Moves the character left and right.
     *
     *  direction is either 1 or -1, to make the character move left or right respectively.
     */
    void move(float deltaTime, std::vector<BadPlatform> platforms);
    
    // TODO: integrate Mei's art with this.
    spine::SkeletonAnimation *ani;
    
    // A box that encompasses the character.
    cocos2d::Vec2 dimensions;
    
    std::string characterName;
    
private:
    void updateAnimation();
    
    State currentState = State::STANDING;
    
    // Vector of x and y velocities, normalized so that 1 moves right/up at max speed, -1 moves left/down at max speed.
    cocos2d::Vec2 velocities = cocos2d::Vec2(0.0, 0.0);
    
    double forceXLeft = 0.0;
    double forceXRight = 0.0;
    
    // The max speed that the character should move laterally in pixels per second.
    cocos2d::Vec2 _maxVelocities;

    double _gravity;
};
} // JTTW

#endif /* Character_h */
