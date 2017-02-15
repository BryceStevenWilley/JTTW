//
//  Piggy.hpp
//  JTTW
//
//  Created by Bryce Willey on 2/15/17.
//
//

#ifndef Piggy_hpp
#define Piggy_hpp


#include <iostream>
#include "Character.hpp"

namespace JTTW {
class Piggy : public Character {
public:
    Piggy(cocos2d::Vec2 startPosition, cocos2d::Size dimensions);
        
    virtual void jump() override;
};
}
#endif /* Piggy_hpp */
