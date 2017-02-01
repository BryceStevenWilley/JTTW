#include "MoveablePlatform.hpp"
#include <iostream>

using namespace JTTW;

MoveablePlatform::MoveablePlatform(std::string &fileName, cocos2d::Vec2 centerA, cocos2d::Vec2 centerB, cocos2d::Size imageSize, cocos2d::Vec2 box, double maxVelocity) : MoveableObject(JTTW::Rectangle(centerA.x, centerA.y, box.x, box.y), cocos2d::Vec2(maxVelocity * (centerB - centerA)/(centerB - centerA).getLength())), image(cocos2d::Sprite::create(fileName)), _centerA(centerA), _centerB(centerB) {
    image->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    updatePosition(_centerA.x, _centerA.y);
    cocos2d::Size actual = image->getContentSize(); // actual image size.
    image->setScaleX(imageSize.width / actual.width);
    image->setScaleY(imageSize.height / actual.height);
    
    
    std::cout << _maxVelocities.x << std::endl;
    std::cout << _maxVelocities.y << std::endl;
    
    // Set initial speed.
    velocities.x = 1.0;
    velocities.y = 1.0;
}

MoveablePlatform::MoveablePlatform(std::string &fileName, cocos2d::Vec2 centerAm, cocos2d::Vec2 centerBm, cocos2d::Size imageSizeM, cocos2d::Vec2 boxM, double maxVelocity, Viewpoint vp) :
        MoveableObject(Rectangle(vp.metersToPixels(centerAm.x), vp.metersToPixels(centerAm.y), vp.metersToPixels(boxM.x), vp.metersToPixels(boxM.y)),
            cocos2d::Vec2(vp.metersToPixels(maxVelocity *((centerBm - centerAm)/(centerBm - centerAm).getLength())))),
        image(cocos2d::Sprite::create(fileName)),
        _centerA(vp.metersToPixels(centerAm)),
        _centerB(vp.metersToPixels(centerBm)) {
    image->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    updatePosition(_centerA.x, _centerA.y);
    cocos2d::Size actual = image->getContentSize(); // actual image size.
    image->setScaleX(vp.metersToPixels(imageSizeM.width) / actual.width);
    image->setScaleY(vp.metersToPixels(imageSizeM.height) / actual.height);
            
    std::cout << _maxVelocities.x << std::endl;
    std::cout << _maxVelocities.y << std::endl;
            
    // Set initial speed.
    velocities.x = 1.0;
    velocities.y = 1.0;
}

void MoveablePlatform::move(float deltaTime, bool debugOn) {
    auto position = dimensions.getCenter();
    //std::cout << "Old position: " << position.x << ", " << position.y << std::endl;
    position.x += velocities.x * _maxVelocities.x * deltaTime;
    position.y += velocities.y * _maxVelocities.y * deltaTime;
    //std::cout << "New position: " << position.x << ", " << position.y << std::endl;
    // Use get lengthSq because it saves up 2 square root operations.
    if (currentState == TOWARDS_B && (position - _centerA).getLengthSq() > (_centerB - _centerA).getLengthSq()) {
        // We've gone too far towards B, so let's turn around again.
        velocities.x = -velocities.x;
        velocities.y = -velocities.y;
    } else if (currentState == TOWARDS_A && (position - _centerB).getLengthSq() > (_centerA - _centerB).getLengthSq()) {
        // We've gone too far towards A, so let's turn around again.
        velocities.x = -velocities.x;
        velocities.y = -velocities.y;
    }
    updatePosition(position.x, position.y);
    if (debugOn) {
        drawCollisionBox();
    }
}

void MoveablePlatform::updatePosition(double centerX, double centerY) {
    dimensions.setCenter(centerX, centerY);
    image->setPosition(centerX, centerY);
}

