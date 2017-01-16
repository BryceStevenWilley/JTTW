//
//  Viewpoint.hpp
//  JTTW
//
//  Created by Bryce Willey on 1/16/17.
//
//

#ifndef Viewpoint_hpp
#define Viewpoint_hpp

#include <iostream>

namespace JTTW {
class Viewpoint {
public:
    Viewpoint(cocos2d::Size screenDims, double metersPerPixel);
    
    int metersToPixels(double meters) const;
    double pixelsToMeters(int pixels) const;
    
private:
    cocos2d::Size _screenDims; // The dimensions of the screen at the time.
    double _metersPerPixel; // The dimensions of the portion of the scene shown on screen in meters
};
};

#endif /* Viewpoint_hpp */
