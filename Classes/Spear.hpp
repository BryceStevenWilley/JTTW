#ifndef Spear_hpp
#define Spear_hpp

#include <iostream>
#include <random>
#include <ctime>
#include "cocos2d.h"

namespace JTTW {

class SpearRNG {
public:
    cocos2d::Vec2 getVelocity(cocos2d::Vec2 spear, cocos2d::Vec2 target);
    cocos2d::Vec2 getPosition(cocos2d::Vec2 target);
    
    SpearRNG() {
        //Initialize with non-deterministic seeds
        rng.seed(std::random_device{}());
    }
    
private:
    //Mersenne Twister.
    std::mt19937 rng;
    std::uniform_real_distribution<double> xVelGen = std::uniform_real_distribution<double>(800, 1000);
    std::uniform_real_distribution<double> yVelGen = std::uniform_real_distribution<double>(0, 200);
};

class Spear : public cocos2d::Sprite {
public:
    Spear(cocos2d::Vec2 targetCenter);
    
private:
    static SpearRNG gen;
};
}

#endif /* Spear_hpp */
