#include "Platform.hpp"
#include "Collisions.hpp"

using namespace JTTW;

Platform::Platform() : image(cocos2d::Sprite::create()) {}

Platform::Platform(std::string &fileName, cocos2d::Vec2 center, cocos2d::Size imageSize, cocos2d::Vec2 box, bool climeable) :
image(cocos2d::Sprite::create(fileName)) {
    image->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    image->setPosition(center);
    image->setContentSize(imageSize);
    
    auto body = cocos2d::PhysicsBody::createBox(cocos2d::Size(box.x, box.y), cocos2d::PhysicsMaterial(1.0, 0.0, 1.0));
    body->setDynamic(false);
    body->setGravityEnable(false);
    body->setTag((int)CollisionCategory::Platform);
    body->setContactTestBitmask((int)CollisionCategory::CharacterAndBoulder);
    body->setCollisionBitmask((int)CollisionCategory::CharacterAndBoulder);
    
    image->addComponent(body);
    
    if (climeable) {
        image->setTag(CLIMBEABLE_TAG);
    }
}
