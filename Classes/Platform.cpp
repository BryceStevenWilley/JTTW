#include "Platform.hpp"
#include "Collisions.hpp"

using namespace JTTW;

Platform::Platform() {
    this->init();
}

double Platform::getYRange() {
    return _yRange;
}

Platform::Platform(std::string &fileName, cocos2d::Vec2 center, 
                   cocos2d::Size imageSize,
                   std::vector<cocos2d::Vec2> points,
                   std::vector<double> frictions,
                   bool climeable,
                   bool collidable) {
    if (!this->initWithFile(fileName)) {
        throw std::invalid_argument(fileName);
    }
    this->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    this->setPosition(center);
    this->setContentSize(imageSize);
    
    if (collidable) {
        int pointCount = (int) points.size();
        body = cocos2d::PhysicsBody::createPolygon(points.data(), pointCount, DEFAULT_MATERIAL);
                
        // Manually set friction.
        if (!frictions.empty()) {
            for (size_t i = 0; i < points.size(); i++) {
                auto segment = cocos2d::PhysicsShapeEdgeSegment::create(points[i % pointCount], points[(i + 1) % pointCount]);
                segment->setFriction(frictions[i]);
                body->addShape(segment);
            }
        }
        
        body->setDynamic(false);
        body->setGravityEnable(false);
        body->setTag((int)CollisionCategory::Platform);
        body->setContactTestBitmask((int)CollisionCategory::CharacterAndBoulder);
        body->setCollisionBitmask((int)CollisionCategory::CharacterAndBoulder);
    
        this->addComponent(body);
    }

    if (climeable) {
        this->setTag(CLIMBEABLE_TAG);
        // Calculate y range.
        double maxHeight = -1 * std::numeric_limits<double>::infinity();
        double minHeight = std::numeric_limits<double>::infinity();
        for (auto &p : points) {
            if (p.y < minHeight) {
                minHeight = p.y;
            }
            if (p.y > maxHeight) {
                maxHeight = p.y;
            }
        }
        _yRange = maxHeight - minHeight;
    }
}
