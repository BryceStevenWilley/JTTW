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
                   std::vector<bool> deathEdges,
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
        
        // Run triangulation.
        body = bodyFromTriangulation(points, DEFAULT_MATERIAL);

        //body = cocos2d::PhysicsBody::createPolygon(points.data(), pointCount, DEFAULT_MATERIAL);
                
        // Manually set friction.
        if (!frictions.empty()) {
            for (size_t i = 0; i < points.size(); i++) {
                auto segment = cocos2d::PhysicsShapeEdgeSegment::create(points[i % pointCount], points[(i + 1) % pointCount]);
                segment->setFriction(frictions[i]);
                body->addShape(segment);
            }
        }
        if (deathEdges.empty()) {
            deathBody = nullptr;
            deathBodyHolder = nullptr;
        } else {
            deathBody = cocos2d::PhysicsBody::create();
            for (size_t i = 0; i < points.size(); i++) {
                if (deathEdges[i]) {
                    cocos2d::PhysicsShapeEdgeSegment *segment = cocos2d::PhysicsShapeEdgeSegment::create(points[i % pointCount], points[(i + 1) % pointCount]);
                    deathBody->addShape(segment);
                }
            }
            deathBody->setDynamic(false);
            deathBody->setGravityEnable(false);
            deathBody->setContactTestBitmask((int)CollisionCategory::CharacterAndBoulder);
            deathBody->setCollisionBitmask((int)CollisionCategory::CharacterAndBoulder);
            deathBodyHolder = cocos2d::Sprite::create();
            deathBodyHolder->setPhysicsBody(deathBody);
            deathBodyHolder->setTag(INSTANT_DEATH_TAG);
            deathBodyHolder->setPosition(center);
            deathBodyHolder->retain();
        }
        
        body->setDynamic(false);
        body->setGravityEnable(false);
        body->setTag((int)CollisionCategory::Platform);
        body->setContactTestBitmask((int)CollisionCategory::CharacterAndBoulder);
        body->setCollisionBitmask((int)CollisionCategory::CharacterAndBoulder);
        
        this->addComponent(body);
    } else {
        deathBody = nullptr;
        deathBodyHolder = nullptr;
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

cocos2d::Sprite *Platform::getDeathBody() {
    return deathBodyHolder;
}

bool Platform::hasDeathBody() {
    return deathBodyHolder == nullptr;
}


// Thanks to Kornel Kisielewicz on Stackoverflow.
// http://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle
float sign (cocos2d::Vec2 p1, cocos2d::Vec2 p2, cocos2d::Vec2 p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool PointInTriangle (cocos2d::Vec2 pt, cocos2d::Vec2* tri)
{
    bool b1, b2, b3;

    b1 = sign(pt, tri[0], tri[1]) < 0.0f;
    b2 = sign(pt, tri[1], tri[2]) < 0.0f;
    b3 = sign(pt, tri[2], tri[0]) < 0.0f;

    return ((b1 == b2) && (b2 == b3));
}

cocos2d::PhysicsBody *Platform::bodyFromTriangulation(std::vector<cocos2d::Vec2> points, cocos2d::PhysicsMaterial mat) {
    // Final shape to add polygons too.
    if (points.size() == 0) {
        std::cerr << "Why are there no points in this platform?" << std::endl;
        throw new std::exception();
    }
    
    auto toBuild = cocos2d::PhysicsBody::create();
    
    // Make a doubly linked list for some reason?
    VertexLink *start = new VertexLink();
    start->vertex = points[0];
    VertexLink *lastMade = start;
    for (int i = 1; i < (int)points.size(); i++) {
        VertexLink *current = new VertexLink();
        current->vertex = points[i];
        current->prev = lastMade;
        lastMade->next = current;
        lastMade = current;
    }
    lastMade->next = start;
    start->prev = lastMade;
    
    // iterate over doubly linked list until you find an ear.
    while (true) {
        bool changed = false;
        VertexLink *current = start;
        // count the number of elements in the list.
        int count = 0;
        do {
            count++;
            current = current->next;
        } while (current != start);
        
        int i = 0;
        do {
            cocos2d::Vec2 array[] = {current->next->vertex, current->vertex, current->prev->vertex};
            bool skip = false;
            for (auto &p : points) {
                if (PointInTriangle(p, array)) {
                    skip = true;
                    break;
                }
            }
            if (!skip) {
                auto tri = cocos2d::PhysicsShapePolygon::create(array, 3);
                toBuild->addShape(tri);
                // Remove 
                current->prev->next = current->next;
                current->next->prev = current->prev;
                if (start == current) {
                    start = current->next;
                }
                changed = true;
            }
            current = current->next;
            i++;
        } while (i < count);
        if (!changed) {
            break;
        }
    }
    
    return toBuild;
}


