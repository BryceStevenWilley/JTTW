#include "SquishObject.hpp"
#include "Collisions.hpp"

using namespace JTTW;


SquishObject::SquishObject(std::string &fileName, cocos2d::Vec2 centerHit, cocos2d::Vec2 centerHold,
        cocos2d::Size imageSize,
        std::vector<cocos2d::Vec2> points,
        std::vector<double> frictions,
        std::vector<bool> deadlySides,
        double slowVelocity,
        double highVelocity,
        double pauseTime) :
        Moveable(fileName, centerHit, centerHold, imageSize, points, frictions, deadlySides, slowVelocity), _vel(slowVelocity), _quickVel(highVelocity), holdTime(pauseTime), accumHeldTime(0.0)
{
   this->setTag(MOVEABLE_TAG);
   _currentState = PREP;
}
    
void SquishObject::move(float deltaTime) {
    std::cout << "This: " << this << std::endl;
    std::cout << "This position x: " << this->getPositionX() << std::endl;
    std::cout << "Current State: " << _currentState << std::endl;
    if (_currentState == PREP && goneFarToB(deltaTime)) {
        setVelocityTowardsA(0.0);
        _currentState = HOLD;
    } else if (_currentState == HOLD) {
        accumHeldTime += deltaTime;
        if (accumHeldTime > holdTime) {
            setVelocityTowardsA(_quickVel);
            accumHeldTime = 0.0;
            _currentState = ATTACK;
        }
    } else { // _currentState == ATTACK
       if (goneFarToA(deltaTime)) {
           setVelocityTowardsB(_vel);
           _currentState = PREP;
       }
    }
}
