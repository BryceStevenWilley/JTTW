#include "Platform.hpp"

using namespace JTTW;

Platform::Platform() : image(cocos2d::Sprite::create()) {}

Platform::Platform(std::string fileName, cocos2d::Vec2 centerMeters, cocos2d::Size imageSizeMeters, cocos2d::Vec2 boxMeters, Viewpoint vp) :
        image(cocos2d::Sprite::create(fileName)) {
    // Convert all of the meter distances into pixels.
    cocos2d::Vec2 center(vp.metersToPixels(centerMeters));
    cocos2d::Size imageSize(vp.metersToPixels(imageSizeMeters.width), vp.metersToPixels(imageSizeMeters.height));
    
    image->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    image->setPosition(center);
    cocos2d::Size actual = image->getContentSize(); // actual image size.
    
    auto body = cocos2d::PhysicsBody::createBox(cocos2d::Size(vp.metersToPixels(boxMeters.x), vp.metersToPixels(boxMeters.y)), cocos2d::PhysicsMaterial(1.0, 0.0, 1.0));
    body->setDynamic(false);
    body->setGravityEnable(false);
    body->setTag(1);
    body->setContactTestBitmask(0xFFFFFFFF);
    
    image->addComponent(body);
    
    image->setScaleX(imageSize.width / actual.width);
    image->setScaleY(imageSize.height / actual.height);
}

Platform::Platform(std::string &fileName, cocos2d::Vec2 center, cocos2d::Size imageSize, cocos2d::Vec2 box) :
image(cocos2d::Sprite::create(fileName)) {
    image->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    image->setPosition(center);
    image->setContentSize(imageSize);
    
    auto body = cocos2d::PhysicsBody::createBox(cocos2d::Size(box.x, box.y), cocos2d::PhysicsMaterial(1.0, 0.0, 0.0));
    body->setDynamic(false);
    body->setGravityEnable(false);
    body->setTag(1);
    body->setContactTestBitmask(0xFFFFFFFF);
    
    image->addComponent(body);
    
}
