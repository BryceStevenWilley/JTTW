#include "Peg.hpp"
#include "Collisions.hpp"
#include "SimpleAudioEngine.h"

using namespace JTTW;

const double PEG_MOVE_AMOUNT = ideal2Res(40);

Peg::Peg(std::string image, cocos2d::Vec2 center, cocos2d::Size imageSize, double rotation, std::vector<Boulder *> bouldersToMakeDynamic): cocos2d::Sprite(),
        _bouldersToMakeDynamic(bouldersToMakeDynamic), _rotation(rotation) {
    auto body = cocos2d::PhysicsBody::createCircle(PEG_USABLE_RADIUS);
    body->setDynamic(false);
    body->setGravityEnable(false);
    body->setTag((int)CollisionCategory::Projectile); //TODO: ???, seems weird to use projectile.
    body->setCollisionBitmask((int)CollisionCategory::None);
    body->setContactTestBitmask((int)CollisionCategory::Character);
    
    this->initWithFile(image);
    this->setTag(PEG_TAG);
    this->setContentSize(imageSize);
    this->setRotation(_rotation);
    this->setPosition(center);
    this->addComponent(body);
}

bool Peg::triggerPeg() {
    if (!_triggered) {
        std::cout << "Triggering peg!" << std::endl;
        _triggered = true;
        // make all of the boulders dynamic.
        for (auto &b : _bouldersToMakeDynamic) {
            b->getBody()->setDynamic(true);
        }
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Sound/BoulderCrumble.wav");
        cocos2d::Vec2 moveDirection(std::cos(3.1415926 * -(_rotation + 90) / 180), std::sin(3.1415926 * -(_rotation + 90) / 180));
        moveDirection *= PEG_MOVE_AMOUNT;
        auto move = cocos2d::MoveBy::create(.3, moveDirection);
        auto fade = cocos2d::FadeOut::create(2.0);
        auto remove = cocos2d::CallFunc::create([this]() {
            this->removeFromParent();
        });
        auto seq = cocos2d::Sequence::create(move, fade, remove, nullptr);
        this->runAction(seq);
        return true;
    }
    return false;
}

void Peg::setToTrigger() {
    _toTrigger = true;
}

bool Peg::isTriggered() const {
    return _triggered;
}

bool Peg::isSetToTrigger() const {
    return _toTrigger;
}
