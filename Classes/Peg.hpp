#ifndef Peg_hpp
#define Peg_hpp

#include <iostream>
#include "cocos2d.h"
#include "Boulder.hpp"
#include "Resolution.hpp"


#include "json.hpp"

namespace JTTW {
const double PEG_USABLE_RADIUS = ideal2Res(60);

class Peg : public cocos2d::Sprite {

public:
    Peg(std::string image, cocos2d::Vec2 center, cocos2d::Size imageSize, double rotation, std::vector<Boulder *> bouldersToMakeDynamic);
    
    bool triggerPeg();
    void setToTrigger();
    bool isTriggered() const;
    bool isSetToTrigger() const;
    
private:
    std::vector<Boulder *> _bouldersToMakeDynamic = std::vector<Boulder *>();
    double _rotation = 0.0;
    bool _triggered = false;
    bool _toTrigger = false;
};
}

#endif /* Peg_hpp */
