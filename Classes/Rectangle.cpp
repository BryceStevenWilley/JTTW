#include "Rectangle.hpp"
#include <iostream>

using namespace JTTW;

Rectangle::Rectangle(cocos2d::Vec2 center, cocos2d::Size dimensions) :
        _center(center), _dimensions(dimensions) {}

Rectangle::Rectangle(double centerX, double centerY, double width, double height) :
        _center(cocos2d::Vec2(centerX, centerY)), _dimensions(cocos2d::Size(width, height)) {}

void Rectangle::setCenter(double x, double y) {
    _center.x = x;
    _center.y = y;
}

bool Rectangle::isInCollisionWith(Rectangle r) const {
    return this->getMaxX() > r.getMinX() && this->getMinX() < r.getMaxX() && // in the same x row.
           this->getMaxY() > r.getMinY() && this->getMinY() < r.getMaxY(); // in the same y column.
}

bool Rectangle::isDirectlyAbove(Rectangle r) const {
    return this->getMaxX() > r.getMinX() && this->getMinX() < r.getMaxX() &&
           this->getMinY() >= r.getMaxY() && this->getMinY() < r.getMaxY() + 5;
}

// Implementation details: to the get closest non colliding point, find the
// different between each possible colliding side (left side of this, right side of r, etc),
// and the position right on the edge of r where this is not colliding for those two sides (do
// we need to move this up, left, right, etc)
//
// Sort to find the smallest difference, and then return that position.
cocos2d::Vec2 Rectangle::closestNonCollidingPoint(Rectangle r) const {
    float bottomDiff = this->getMaxY() - r.getMinY();
    float topDiff = r.getMaxY() - this->getMinY();
    float leftDiff = this->getMaxX() - r.getMinX();
    float rightDiff = r.getMaxX() - this->getMinX();
    
    std::vector<float> diffs;
    diffs.push_back(bottomDiff);
    diffs.push_back(topDiff);
    diffs.push_back(leftDiff);
    diffs.push_back(rightDiff);
    std::map<float, cocos2d::Vec2> closestPoint;
    closestPoint[bottomDiff] = cocos2d::Vec2(this->getCenterX(), r.getMinY() -this->getHeight() / 2.0);
    closestPoint[topDiff] = cocos2d::Vec2(this->getCenterX(), r.getMaxY() + this->getHeight() / 2.0);
    closestPoint[leftDiff] = cocos2d::Vec2(r.getMinX() -this->getWidth() / 2.0,this->getCenterY());
    closestPoint[rightDiff] = cocos2d::Vec2(r.getMaxX() +this->getWidth() / 2.0,this->getCenterY());
    
    std::sort(diffs.begin(), diffs.end());
    
    return closestPoint[diffs[0]];
}

std::array<cocos2d::Vec2, 4> Rectangle::getPoints() const {
    std::array<cocos2d::Vec2, 4> rect;
    rect[0] = cocos2d::Vec2(getMinX(), getMinY());
    rect[1] = cocos2d::Vec2(getMaxX(), getMinY());
    rect[2] = cocos2d::Vec2(getMaxX(), getMaxY());
    rect[3] = cocos2d::Vec2(getMinX(), getMaxY());
    return rect;
}

const cocos2d::Vec2 Rectangle::getCenter() const {
    return cocos2d::Vec2(_center);
}

double Rectangle::getMaxX() const {
    return _center.x + _dimensions.width / 2.0;
}

double Rectangle::getMinX() const {
    return _center.x - _dimensions.width / 2.0;
}

double Rectangle::getMaxY() const {
    return _center.y + _dimensions.height / 2.0;
}

double Rectangle::getMinY() const {
    return _center.y - _dimensions.height / 2.0;
}

double Rectangle::getCenterX() const {
    return _center.x;
}

double Rectangle::getCenterY() const {
    return _center.y;
}

double Rectangle::getWidth() const {
    return _dimensions.width;
}
double Rectangle::getHeight() const {
    return _dimensions.height;
}
