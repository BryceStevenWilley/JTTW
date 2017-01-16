//
//  Viewpoint.cpp
//  JTTW
//
//  Created by Bryce Willey on 1/16/17.
//
//

#include "Viewpoint.hpp"

using namespace JTTW;

Viewpoint::Viewpoint(cocos2d::Size dims, double metersPerPixel) :
        _screenDims(dims), _metersPerPixel(metersPerPixel) {}

int Viewpoint::metersToPixels(double meters) const {
    return meters / _metersPerPixel;
}

double Viewpoint::pixelsToMeters(int pixels) const {
    return pixels * _metersPerPixel;
}
