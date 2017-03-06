#ifndef Peg_hpp
#define Peg_hpp

#include <iostream>
#include "cocos2d.h"
#include "Boulder.hpp"

#include "json.hpp"

namespace JTTW {
const double PEG_USABLE_RADIUS = 100;

class Peg : public cocos2d::Sprite {

public:
    Peg(std::string image, cocos2d::Vec2 center, cocos2d::Size imageSize, double rotation, std::vector<Boulder *> bouldersToMakeDynamic);
    
    bool triggerPeg(cocos2d::Vec2 characterCenter);
    
private:
    std::vector<Boulder *> _bouldersToMakeDynamic = std::vector<Boulder *>();
    double _rotation = 0.0;
    bool _triggered = false;
};
}

#endif /* Peg_hpp */
