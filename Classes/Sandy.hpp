#ifndef Sandy_hpp
#define Sandy_hpp

#include <iostream>
#include "Character.hpp"

namespace JTTW {
class Sandy : public Character {
public:
    static const double JUMP_INIT;
    Sandy(cocos2d::Vec2 startPosition, cocos2d::Size dimensions);
        
    virtual void initJump() override;
    virtual void characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) override;
        
    virtual void callHey() override;
};
}
#endif /* Sandy_hpp */
