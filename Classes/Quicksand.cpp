//
//  Quicksand.cpp
//  JTTW
//
//  Created by Bryce Willey on 4/10/17.
//
//

#include "Quicksand.hpp"
#include "Collisions.hpp"

using namespace JTTW;

Quicksand::Quicksand(std::string &fileName, cocos2d::Vec2 center,
                   cocos2d::Size imageSize,
                   std::vector<cocos2d::Vec2> points,
                   double sinkVel,
                   double recoverVel) : _sinkVel(sinkVel), _recoverVel(recoverVel) {

    if (!this->initWithFile(fileName)) {
        throw std::invalid_argument(fileName);
    }
    this->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    this->setPosition(center);
    this->setContentSize(imageSize);
    
    int pointCount = (int) points.size();

    body = cocos2d::PhysicsBody::createPolygon(points.data(), pointCount);
    body->getFirstShape()->setSensor(true);
    body->setDynamic(false);
    body->setGravityEnable(false);
    body->setTag((int)CollisionCategory::Platform);
    body->setContactTestBitmask((int)CollisionCategory::CharacterAndBoulder);
    body->setCollisionBitmask((int)CollisionCategory::CharacterAndBoulder);
    
    this->addComponent(body);
    this->setTag(QUICKSAND_TAG);
}


