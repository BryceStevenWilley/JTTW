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
    std::cout << "Meters: " << meters << std::endl;
    std::cout << "Corversion: " << 1.0 / _metersPerPixel << std::endl;
    std::cout << "Pixels: " << meters / _metersPerPixel << std::endl;
    std::cout << "Returned : " << int(meters / _metersPerPixel) << std::endl;
    return meters / _metersPerPixel;
}

double Viewpoint::pixelsToMeters(int pixels) const {
    std::cout << "Pixels: " << pixels << std::endl;
    std::cout << "Corversion: " << _metersPerPixel << std::endl;
    std::cout << "Meters: " << pixels * _metersPerPixel << std::endl;
    std::cout << "Returned : " << double(pixels * _metersPerPixel) << std::endl;
    return pixels * _metersPerPixel;
}
