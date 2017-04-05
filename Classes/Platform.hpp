#ifndef Platform_hpp
#define Platform_hpp

#include "Viewpoint.hpp"
#include "cocos2d.h"
#include "SceneObject.hpp"

namespace JTTW {
    
// Forward declaration of Viewpoint.
class Viewpoint;
    
class Platform: public SceneObject {
public:
    Platform();

    Platform(std::string &fileName, cocos2d::Vec2 center, cocos2d::Size imageSize, std::vector<cocos2d::Vec2> points, std::vector<double> frictions, bool climeable, bool collidable);
    
    virtual double getYRange() override;
protected:
    cocos2d::PhysicsBody *body;
    
private:
    const cocos2d::PhysicsMaterial DEFAULT_MATERIAL = cocos2d::PhysicsMaterial(1.0, 0.0, 1.0);
    double _yRange;
};
}
#endif /* Platform_hpp */
