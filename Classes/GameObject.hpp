#ifndef GameObject_hpp
#define GameObject_hpp

#include "Rectangle.hpp"

namespace JTTW {

class GameObject {
public:
    GameObject(Rectangle dimensions);
    
    // A box that encompasses the character.
    Rectangle dimensions;
    
    cocos2d::DrawNode *collisionBoxNode;
    
protected:
    void drawCollisionBox();
    
};
}

#endif /* GameObject_hpp */
