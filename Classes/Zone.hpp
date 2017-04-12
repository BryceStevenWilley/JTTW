//
//  Zone.hpp
//  JTTW
//
//  Created by Bryce Willey on 4/3/17.
//
//

#ifndef Zone_hpp
#define Zone_hpp

#include "ProjectileFactory.hpp"

namespace JTTW {
struct Zone {
private:
    cocos2d::Vec2 bottomLeft;
    cocos2d::Vec2 topRight;
    ProjectileFactory *fac;
    
public:
    Zone(cocos2d::Vec2 a, cocos2d::Vec2 b, ProjectileFactory *f): bottomLeft(a), topRight(b), fac(f) {}
    
    bool containsPoint(cocos2d::Vec2 p) {
        return (p.x >= bottomLeft.x && p.x <= topRight.x && p.y >= bottomLeft.y && p.y <= topRight.y);
    }
    
    ProjectileFactory *getFactory() { return fac; }
};
}

#endif /* Zone_hpp */
