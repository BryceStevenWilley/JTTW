#ifndef Monk_hpp
#define Monk_hpp

#include <iostream>
#include "Character.hpp"
#include "Peg.hpp"

namespace JTTW {
class Monk : public Character {
public:
    Monk(cocos2d::Vec2 startPosition, cocos2d::Size dimensions);
    
    virtual void jump() override;
    virtual void characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) override;
    
    void addReachiblePeg(Peg *p);
    
private:
    Peg *reachiblePeg = nullptr;
    
};
}
#endif /* Monk_hpp */
