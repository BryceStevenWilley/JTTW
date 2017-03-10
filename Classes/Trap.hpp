#ifndef Trap_hpp
#define Trap_hpp

#include "cocos2d.h"
#include "Character.hpp"
#include <iostream>

namespace JTTW {
class Trap {
public:
    /** Returns true if trap was triggered. */
    virtual bool triggerTrap(cocos2d::Vec2 characterCenter, cocos2d::Size characterSize) = 0;
    
    virtual Action characterReaction() const = 0;
};
}

#endif /* Trap_hpp */
