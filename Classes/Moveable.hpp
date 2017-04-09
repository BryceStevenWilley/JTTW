#ifndef Moveable_hpp
#define Moveable_hpp

#include <iostream>
#include "Platform.hpp"

namespace JTTW {

const bool NOT_CLIMABLE = false;
const bool COLLIDABLE = true;

class Moveable: public Platform {
public:
    Moveable(std::string &fileName, cocos2d::Vec2 centerA, cocos2d::Vec2 centerB,
                                   cocos2d::Size imageSize,
                                   std::vector<cocos2d::Vec2> points,
                                   std::vector<double> frictions,
                                   std::vector<bool> deadlySides,
                                   double maxVelocity)
        : Platform(fileName, centerA, imageSize, points, frictions, deadlySides, NOT_CLIMABLE, COLLIDABLE),
        _centerA(centerA), _centerB(centerB) {
            // Set initial speed.
            setVelocityTowardsB(maxVelocity);
        }
    
    virtual void move(float delta) = 0;

    bool goneFarToB(float deltaTime) {
        auto position = this->getPosition() + _velocities * deltaTime;
        return (position - _centerA).getLengthSq() > (_centerB - _centerA).getLengthSq();
    }
    
    bool goneFarToA(float deltaTime) {
        auto position = this->getPosition() + _velocities * deltaTime;
        return (position - _centerB).getLengthSq() > (_centerA - _centerB).getLengthSq();
    }
    
    void setVelocityTowardsA(double vel) {
        _velocities = vel * (_centerA - _centerB)/(_centerA - _centerB).getLength();
        deathBody->setVelocity(_velocities);
        body->setVelocity(_velocities);
    }
    
    void setVelocityTowardsB(double vel) {
        _velocities = vel * (_centerB - _centerA)/(_centerB - _centerA).getLength();
        deathBody->setVelocity(_velocities);
        body->setVelocity(_velocities);
    }

protected:
    cocos2d::Vec2 _centerA;
    cocos2d::Vec2 _centerB;
    
    // Current Velocities.
    cocos2d::Vec2 _velocities;
};

}

#endif /* Moveable_hpp */
