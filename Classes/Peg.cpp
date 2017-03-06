#include "Peg.hpp"
#include "Collisions.hpp"

using namespace JTTW;

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
    this->setRotation(rotation);
    this->setPosition(center);
    this->addComponent(body);
}

bool Peg::triggerPeg(cocos2d::Vec2 characterCenter) {
    if (!_triggered) { // && this->getPosition().getDistance(characterCenter) < PEG_USABLE_RADIUS) {
        std::cout << "Triggering peg!" << std::endl;
        // make all of the boulders dynamic.
        for (auto &b : _bouldersToMakeDynamic) {
            b->getBody()->setDynamic(true);
            //b->getBody()->setContactTestBitmask((int)CollisionCategory::CharacterPlatformAndBoulder);
            //b->getBody()->setCollisionBitmask((int)CollisionCategory::CharacterPlatformAndBoulder);
        }
        cocos2d::Vec2 moveDirection(0, 40);
        moveDirection.rotateByAngle(cocos2d::Vec2::ZERO, _rotation);
        auto move = cocos2d::MoveBy::create(.3, moveDirection);
        auto fade = cocos2d::FadeOut::create(2.0);
        auto remove = cocos2d::CallFunc::create([this]() {
            _triggered = true;
        });
        auto seq = cocos2d::Sequence::create(move, fade, remove, nullptr);
        this->runAction(seq);
        return true;
    }
    std::cout << "Not triggered!: _triggered = " << _triggered << std::endl;
    return false;
}
