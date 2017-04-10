#include "SinkObject.hpp"
#include "Collisions.hpp"

using namespace JTTW;

SinkObject::SinkObject(std::string &fileName, cocos2d::Vec2 centerA, cocos2d::Vec2 centerB,
        cocos2d::Size imageSize,
        std::vector<cocos2d::Vec2> points,
        std::vector<double> frictions,
        std::vector<bool> deadlySides,
        double velocity) :
        Moveable(fileName, centerB, centerA, imageSize, points, frictions, deadlySides, 0.0), _vel(velocity)
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
    countOn += 1; 
    _currentState = SINKING;
    setVelocityTowardsB(_vel);
}

void SinkObject::removeCallback() {
    countOn -= 1;
    if (countOn == 0) {
        _currentState = FLOATING;
        setVelocityTowardsA(_vel);
    } else if (countOn < 0) {
        std::cerr << "Sink object has negative people on it?" << std::endl;
        countOn = 0;
    }
}

