#ifndef Platform_hpp
#define Platform_hpp

#include "Viewpoint.hpp"
#include "cocos2d.h"
#include "SceneObject.hpp"

namespace JTTW {
    
// Forward declaration of Viewpoint.
class Viewpoint;
    
struct VertexLink {
    VertexLink *prev;
    VertexLink *next;
    cocos2d::Vec2 vertex;
};
    
    
class Platform: public SceneObject {
public:
    Platform();

    Platform(std::string &fileName, cocos2d::Vec2 center, cocos2d::Size imageSize, std::vector<cocos2d::Vec2> points, std::vector<double> frictions, std::vector<bool> deathEdges, bool climeable, bool collidable);
    
    cocos2d::Sprite *getDeathBody();
    bool hasDeathBody();
    
    virtual double getYRange() override;
protected:
    cocos2d::PhysicsBody *body;
    cocos2d::Sprite *deathBodyHolder;
    cocos2d::PhysicsBody *deathBody;
    
private:
    // MUST BE IN COUNTER-CLOCK-WISE ORDER.
    cocos2d::PhysicsBody *bodyFromTriangulation(std::vector<cocos2d::Vec2> points, cocos2d::PhysicsMaterial mat);


    const cocos2d::PhysicsMaterial DEFAULT_MATERIAL = cocos2d::PhysicsMaterial(1.0, 0.0, 1.0);
    double _yRange;
};
}
#endif /* Platform_hpp */
