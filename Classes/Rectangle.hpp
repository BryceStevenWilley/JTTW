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
    
    double getMaxY();
    double getMinY();
    double getMaxX();
    double getMinX();
    
    double getCenterX();
    double getCenterY();
    double getWidth();
    double getHeight();
};
}


#endif /* Rectangle_hpp */
