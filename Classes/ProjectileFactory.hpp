#ifndef ProjectileFactory_hpp
#define ProjectileFactory_hpp

#include <iostream>
#include "cocos2d.h"
#include "json.hpp"
#include "Viewpoint.hpp"

namespace JTTW {

class ProjectileFactory {
public:
    ProjectileFactory(cocos2d::Vec2 maxVel, cocos2d::Vec2 minVel, std::string assetName, cocos2d::Size assetSize, std::string soundName);
    virtual cocos2d::Sprite *generateProjectile(cocos2d::Vec2 targetCenter) = 0;
    
protected:
    virtual cocos2d::Sprite *getEmptyProjectile();
    
    std::mt19937 rng;
    std::uniform_real_distribution<double> xVelGen;
    std::uniform_real_distribution<double> yVelGen;
    
private:
    std::string _assetName;
    std::string _soundName;
    cocos2d::Size _assetSize;
};

class RelativeProjectileFactory : public ProjectileFactory {
public:
    RelativeProjectileFactory(std::string assetName, cocos2d::Size assetSize, std::string soundName, cocos2d::Vec2 maxVel, cocos2d::Vec2 minVel, cocos2d::Vec2 maxOffset, cocos2d::Vec2 minOffset);
    
    cocos2d::Sprite *generateProjectile(cocos2d::Vec2 targetCenter);

private:
    std::uniform_real_distribution<double> xOffsetGen;
    std::uniform_real_distribution<double> yOffsetGen;
};

class AbsoluteProjectileFactory : public ProjectileFactory {
public:
    AbsoluteProjectileFactory(std::string assetName, cocos2d::Size assetSize, std::string sound, cocos2d::Vec2 maxVel, cocos2d::Vec2 minVel, cocos2d::Vec2 position);
    
    cocos2d::Sprite *generateProjectile(cocos2d::Vec2 targetCenter);
    
private:
    cocos2d::Vec2 _position;
};

ProjectileFactory *createFactoryFromJson(nlohmann::json projInfo, Viewpoint vp);

}

#endif /* ProjectileFactory_hpp */
