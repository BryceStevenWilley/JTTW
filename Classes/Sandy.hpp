#ifndef Sandy_hpp
#define Sandy_hpp

#include <iostream>
#include "Character.hpp"

namespace JTTW {
class Sandy : public Character {
public:
    Sandy(cocos2d::Vec2 startPosition, cocos2d::Size dimensions);
        
    virtual void jump() override;
    virtual void characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) override;
    
};
}
#endif /* Sandy_hpp */
