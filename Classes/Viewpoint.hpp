#ifndef Viewpoint_hpp
#define Viewpoint_hpp

#include <stdio.h>
#include "Character.hpp"

namespace JTTW {
    
// Forward declaration of Character.
class Character;

class Viewpoint {
public:
    Viewpoint(cocos2d::Size screenDims, double metersPerPixel, cocos2d::Layer *level);
    
    void panToCharacter(Character *player);
    void followCharacter(Character *player, float delta);
    void followCharacter(cocos2d::Node *body, float delta);
    
    int metersToPixels(double meters) const;
    cocos2d::Vec2 metersToPixels(cocos2d::Vec2 meters) const;
    double pixelsToMeters(int pixels) const;
    
private:
    cocos2d::Size _screenDims; // The dimensions of the screen at the time.
    double _metersPerPixel; // The dimensions of the portion of the scene shown on screen in meters
    
    cocos2d::Layer *_level; // get from the level layer->getPosition.
    
    bool _isPanning = false;
};
};

#endif /* Viewpoint_hpp */
