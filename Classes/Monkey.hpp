//
//  Monkey.hpp
//  JTTW
//
//  Created by Bryce Willey on 2/15/17.
//
//

#ifndef Monkey_hpp
#define Monkey_hpp

#include <iostream>
#include "Character.hpp"

namespace JTTW {
class Monkey : public Character {
public:
    Monkey(cocos2d::Vec2 startPosition, cocos2d::Size dimensions);
    
    virtual void jump() override;
};
}

#endif /* Monkey_hpp */
