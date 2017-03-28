#include "ProjectileFactory.hpp"
#include "SimpleAudioEngine.h"
#include "Collisions.hpp"

using namespace JTTW;

ProjectileFactory::ProjectileFactory(cocos2d::Vec2 maxVel, cocos2d::Vec2 minVel, std::string assetName, cocos2d::Size assetSize, std::string soundName, bool dynamic) :
        xVelGen(minVel.x, maxVel.x),
        yVelGen(minVel.y, maxVel.y),
        _assetName(assetName),
        _soundName(soundName),
        _assetSize(assetSize),
        _dynamic(dynamic) {
    rng.seed(std::random_device{}());
}

cocos2d::Sprite *ProjectileFactory::getEmptyProjectile() {
    auto sprite = cocos2d::Sprite::create(_assetName); // "assets/spear.png");
    // Throw a projectile somewhere!
    auto body = cocos2d::PhysicsBody::createBox(_assetSize);
    body->setDynamic(true);
    
    body->setGravityEnable(true);
    body->setTag((int)CollisionCategory::Projectile);
    body->setCollisionBitmask((int)CollisionCategory::None);
    body->setContactTestBitmask((int)CollisionCategory::Character);
    body->setDynamic(_dynamic);
    
    sprite->setContentSize(_assetSize);
    sprite->addComponent(body);
    sprite->setTag(PROJECTILE_TAG);
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(_soundName.c_str());
    
    return sprite;
}

RelativeProjectileFactory::RelativeProjectileFactory(std::string assetName, cocos2d::Size assetSize, std::string soundName, cocos2d::Vec2 maxVel, cocos2d::Vec2 minVel, cocos2d::Vec2 maxOffset, cocos2d::Vec2 minOffset, bool dynamic) :
        ProjectileFactory(maxVel, minVel, assetName, assetSize, soundName, dynamic),
        xOffsetGen(minOffset.x, maxOffset.x),
        yOffsetGen(minOffset.y, maxOffset.y) {}

cocos2d::Sprite *RelativeProjectileFactory::generateProjectile(cocos2d::Vec2 targetCenter) {
    auto sprite = getEmptyProjectile();
    sprite->setPosition(targetCenter + cocos2d::Vec2(xOffsetGen(rng), yOffsetGen(rng)));
    sprite->getPhysicsBody()->setVelocity(cocos2d::Vec2(xVelGen(rng), yVelGen(rng)));
    return sprite;
}

AbsoluteProjectileFactory::AbsoluteProjectileFactory(std::string assetName, cocos2d::Size assetSize, std::string sound, cocos2d::Vec2 maxVel, cocos2d::Vec2 minVel, cocos2d::Vec2 position, bool dynamic) :
        ProjectileFactory(maxVel, minVel, assetName, assetSize, sound, dynamic),
        _position(position) {}

cocos2d::Sprite *AbsoluteProjectileFactory::generateProjectile(cocos2d::Vec2 targetCenter) {
    auto sprite = getEmptyProjectile();
    sprite->setPosition(_position);
    sprite->getPhysicsBody()->setVelocity(cocos2d::Vec2(xVelGen(rng), yVelGen(rng)));
    return sprite;
}

ProjectileFactory *JTTW::createFactoryFromJson(nlohmann::json projInfo, Viewpoint vp) {
    double xVelMax = projInfo["book"]["doubList"]["xVelMax"];
    double yVelMax = projInfo["book"]["doubList"]["yVelMax"];
    double xVelMin = projInfo["book"]["doubList"]["xVelMin"];
    double yVelMin = projInfo["book"]["doubList"]["yVelMin"];
    // TODO: use the meters to pixels thing when needed.
    auto maxVel = cocos2d::Vec2(xVelMax, yVelMax);
    auto minVel = cocos2d::Vec2(xVelMin, yVelMin);
             
    double imageWidth = vp.metersToPixels((double)projInfo["scaledIGWM"]);
    double imageHeight = vp.metersToPixels((double)projInfo["scaledIGHM"]);
    auto assetSize = cocos2d::Size(imageWidth, imageHeight);
            
    std::string imageName = projInfo["path"];
    imageName = "assets/" + imageName;
    std::string soundName = projInfo["book"]["strList"]["soundName"];
    soundName = "Sound/" + soundName;
    
    bool dynamic = projInfo["book"]["boolList"]["dynamic"];
    
    if (projInfo["book"]["strList"]["FireType"].is_string()) {
        std::string type = projInfo["book"]["strList"]["FireType"];
        std::transform(type.begin(), type.end(), type.begin(), ::toupper);
        if (type == "RELATIVE") {
            double xOffsetMax = projInfo["book"]["doubList"]["xOffsetMax"];
            double yOffsetMax = projInfo["book"]["doubList"]["yOffsetMax"];
            double xOffsetMin = projInfo["book"]["doubList"]["xOffsetMin"];
            double yOffsetMin = projInfo["book"]["doubList"]["yOffsetMin"];
            auto maxOffset = cocos2d::Vec2(xOffsetMax, yOffsetMax);
            auto minOffset = cocos2d::Vec2(xOffsetMin, yOffsetMin);
   
            return new RelativeProjectileFactory(imageName, assetSize, soundName, maxVel, minVel, maxOffset, minOffset, dynamic);
        } else if (type == "ABSOLUTE") {
            double positionX = projInfo["centerXM"];
            double positionY = projInfo["centerYM"];
            auto position = vp.metersToPixels(cocos2d::Vec2(positionX, positionY));
            
            return new AbsoluteProjectileFactory(imageName, assetSize, soundName, maxVel, minVel, position, dynamic);
        } else {
            std::cout << "FireType should be either RELATIVE or ABSOLUTE, not " << type << std::endl;
            throw std::invalid_argument("Projectile type should be relative or absolute.");
        }
    } else {
        throw std::domain_error("Projectiles must have a \"FireType\"");
    }
}


