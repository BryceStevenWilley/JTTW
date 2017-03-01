#ifndef Boulder_hpp
#define Boulder_hpp

#include <iostream>

#endif /* Boulder_hpp */

namespace JTTW {

class Boulder : public cocos2d::Sprite {
public:
    Boulder(double radius, std::string imageName, cocos2d::Vec2 center, double mass, cocos2d::Size imgSize);
    Boulder(std::vector<cocos2d::Vec2> points, std::string imageName, cocos2d::Vec2 center, double mass, cocos2d::Size imgSize);
};

}
