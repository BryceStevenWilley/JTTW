#ifndef Piggy_hpp
#define Piggy_hpp


#include <iostream>
#include "Character.hpp"

namespace JTTW {

/**
 * The code for Piggy specific controls and mechanics.
 *    
 * The main mechanic is the "Boulder Stance", indicated by the boulderMode flag.
 * If boulderMode is true, Piggy won't move, but other characters can now stand on him.
 * The mechanic is toggled by pressing 's'.
 */
class Piggy : public Character {
public:
    static const double JUMP_INIT;
    
    enum State {
        NORMAL,
        BOULDER_MODE,
    };

    Piggy(cocos2d::Vec2 startPosition, cocos2d::Size dimensions);
    
    virtual void impulseLeft(float deltaVel) override;
    virtual void impulseRight(float deltaVel) override;
    virtual void initJump() override;
    virtual void characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) override;
    
    void addHeldCharacter(Character *c);
    void removeHeldCharacter(Character *c);
    void launchHeldCharacters();
    
private:
    State _state = NORMAL;
    
    std::vector<Character *> _heldCharacters = std::vector<Character *>();
};
}
#endif /* Piggy_hpp */
