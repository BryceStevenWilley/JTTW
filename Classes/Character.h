#ifndef Character_h
#define Character_h

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
    Character(const std::string artFileName);
    ~Character();
    
    State currentState = State::STANDING;
    
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
    
    // The max speed that the character should move laterally in pixels per second.
    cocos2d::Vec2 maxVelocities = cocos2d::Vec2(400.0, 400.0);
    
    // Vector of x and y velocities, normalized so that 1 moves right/up at max speed, -1 moves left/down at max speed.
    cocos2d::Vec2 velocities = cocos2d::Vec2(0.0, 0.0);
};
}; // JTTW

#endif /* Character_h */
