#include "SinkObject.hpp"
#include "Collisions.hpp"

using namespace JTTW;

SinkObject::SinkObject(std::string &fileName, cocos2d::Vec2 centerA, cocos2d::Vec2 centerB,
        cocos2d::Size imageSize,
        std::vector<cocos2d::Vec2> points,
        std::vector<double> frictions,
        std::vector<bool> deadlySides,
        double velocity,
        double pauseTime) :
        Moveable(fileName, centerA, centerB, imageSize, points, frictions, deadlySides, velocity), _vel(velocity)
{
   this->setTag(SINKABLE_TAG);
   _currentState = PEAK;
}
    
void SinkObject::move(float deltaTime) {
    if (_currentState == FLOATING) {
        if (goneFarToA(deltaTime)) {
            _currentState = PEAK;
            setVelocityTowardsB(0.0);
        }
    } else if (_currentState == SINKING) {
        if (goneFarToB(deltaTime)) {
            _currentState = TROUGH;
            setVelocityTowardsA(0.0);
        }
    }
}

void SinkObject::landedCallback() {
    _currentState = SINKING;
    setVelocityTowardsB(_vel);
}

void SinkObject::removeCallback() {
    _currentState = FLOATING;
    setVelocityTowardsA(_vel);
}

