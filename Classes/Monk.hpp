#ifndef Monk_hpp
#define Monk_hpp

#include <iostream>
#include "Character.hpp"

namespace JTTW {
class Monk : public Character {
public:
    Monk(cocos2d::Vec2 startPosition, cocos2d::Size dimensions);
    
    virtual void jump() override;
    virtual void characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) override;
    
};
}
#endif /* Monk_hpp */
