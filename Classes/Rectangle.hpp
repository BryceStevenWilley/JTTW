//
//  Rectangle.hpp
//  JTTW
//
//  Created by Bryce Willey on 1/27/17.
//
//

#ifndef Rectangle_hpp
#define Rectangle_hpp

#include <stdio.h>
#include <array>
#include "cocos2d.h"

namespace JTTW {
class Rectangle {
private:
    cocos2d::Vec2 _center;
    cocos2d::Size _dimensions;
    
public:
    Rectangle(cocos2d::Vec2 center, cocos2d::Size dimensions);
    Rectangle(double centerX, double centerY, double width, double height);
    
    void setCenter(double x, double y);
    
    const cocos2d::Vec2 getCenter() const;
    
    bool isInCollisionWith(Rectangle r) const;
    bool isDirectlyAbove(Rectangle r) const;
    
    // Assumes that the current rectangle is moveable, and the given rectangle is stationary.
    cocos2d::Vec2 closestNonCollidingPoint(Rectangle r) const;
    
    std::array<cocos2d::Vec2, 4> getPoints() const;
    
    double getMaxY() const;
    double getMinY() const;
    double getMaxX() const;
    double getMinX() const;
    
    double getCenterX() const;
    double getCenterY() const;
    double getWidth() const;
    double getHeight() const;
};
}


#endif /* Rectangle_hpp */
