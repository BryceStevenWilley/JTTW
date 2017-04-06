#ifndef Resolution_hpp
#define Resolution_hpp

#include <iostream>
#include "cocos2d.h"

namespace JTTW {
    static const cocos2d::Size tinyResolution = cocos2d::Size(480, 270);
    static const cocos2d::Size smallResolution = cocos2d::Size(512, 288);
    static const cocos2d::Size mediumResolution = cocos2d::Size(1024, 576);
    static const cocos2d::Size lardiumResolution = cocos2d::Size(1560, 878);
    static const cocos2d::Size largeResolution = cocos2d::Size(2048, 1152);
    
    // Here for backwards compability with stuff in AppDelegate.
    static const cocos2d::Size designResolution =  mediumResolution;
    
    // The resolution for which all of the constant values are coded for.
    static const cocos2d::Size idealResolution = lardiumResolution;
    
    // The resolution being run on.
    static const cocos2d::Size actualResolution = lardiumResolution;
    
    static const double screenScale = actualResolution.width / idealResolution.width;
    
    double ideal2Res(double ideal);
};

#endif /* Resolution_hpp */
