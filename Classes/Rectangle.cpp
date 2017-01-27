//
//  Rectangle.cpp
//  JTTW
//
//  Created by Bryce Willey on 1/27/17.
//
//

#include "Rectangle.hpp"

using namespace JTTW;

Rectangle::Rectangle(cocos2d::Vec2 center, cocos2d::Size dimensions) :
        _center(center), _dimensions(dimensions) {}

Rectangle::Rectangle(double centerX, double centerY, double width, double height) :
        _center(cocos2d::Vec2(centerX, centerY)), _dimensions(cocos2d::Size(width, height)) {}

void Rectangle::setCenter(double x, double y) {
    _center.x = x;
    _center.y = y;
}

double Rectangle::getMaxX() {
    return _center.x + _dimensions.width / 2.0;
}

double Rectangle::getMinX() {
    return _center.x - _dimensions.width / 2.0;
}

double Rectangle::getMaxY() {
    return _center.y + _dimensions.height / 2.0;
}

double Rectangle::getMinY() {
    return _center.y - _dimensions.height / 2.0;
}

double Rectangle::getCenterX() {
    return _center.x;
}

double Rectangle::getCenterY() {
    return _center.y;
}

double Rectangle::getWidth() {
    return _dimensions.width;
}
double Rectangle::getHeight() {
    return _dimensions.height;
}
