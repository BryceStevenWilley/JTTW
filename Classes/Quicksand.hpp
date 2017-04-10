//
//  Quicksand.hpp
//  JTTW
//
//  Created by Bryce Willey on 4/10/17.
//
//

#ifndef Quicksand_hpp
#define Quicksand_hpp

#include <iostream>
#include "cocos2d.h"

namespace JTTW {

class Quicksand : public cocos2d::Sprite {
public:
    Quicksand(std::string &fileName, cocos2d::Vec2 center,
                   cocos2d::Size imageSize,
                   std::vector<cocos2d::Vec2> points,
                   double sinkVel,
                   double recoverVel);
    
    const double _sinkVel;
    const double _recoverVel;
    
protected:
    cocos2d::PhysicsBody *body;
};

}

#endif /* Quicksand_hpp */
