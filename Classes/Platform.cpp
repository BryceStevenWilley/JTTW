#include "Platform.hpp"

using namespace JTTW;

Platform::Platform() : GameObject(Rectangle(cocos2d::Vec2(), cocos2d::Size())), image(cocos2d::Sprite::create()) {}

Platform::Platform(std::string fileName, cocos2d::Vec2 centerMeters, cocos2d::Size imageSizeMeters, cocos2d::Vec2 boxMeters, Viewpoint vp) : GameObject(Rectangle(vp.metersToPixels(centerMeters.x), vp.metersToPixels(centerMeters.y), vp.metersToPixels(boxMeters.x), vp.metersToPixels(boxMeters.y))),
        image(cocos2d::Sprite::create(fileName)) {
    // Convert all of the meter distances into pixels.
    cocos2d::Vec2 center(vp.metersToPixels(centerMeters.x), vp.metersToPixels(centerMeters.y));
    cocos2d::Size imageSize(vp.metersToPixels(imageSizeMeters.width), vp.metersToPixels(imageSizeMeters.height));
    
    image->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    image->setPosition(center);
    cocos2d::Size actual = image->getContentSize(); // actual image size.
    image->setScaleX(imageSize.width / actual.width);
    image->setScaleY(imageSize.height / actual.height);
}

Platform::Platform(std::string &fileName, cocos2d::Vec2 center, cocos2d::Size imageSize, cocos2d::Vec2 box) :
image(cocos2d::Sprite::create(fileName)), GameObject(Rectangle(center, cocos2d::Size(box.x, box.y))) {
    image->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    image->setPosition(center);
    image->setContentSize(imageSize);
}
