#include "Monk.hpp"

using namespace JTTW;

Monk::Monk(cocos2d::Vec2 startPosition, cocos2d::Size dimensions) :
 Character("Monk", cocos2d::PhysicsMaterial(1.0, 0.0, 0.0), startPosition, dimensions) {}

void Monk::jump() {
    Character::jump(400);
}


void Monk::characterSpecial(cocos2d::EventKeyboard::KeyCode code, bool pressed) {
    if (pressed && code == cocos2d::EventKeyboard::KeyCode::KEY_S && reachiblePeg != nullptr) {
        reachiblePeg->triggerPeg(this->getPosition());
        reachiblePeg = nullptr;
    }
}

void Monk::addReachiblePeg(Peg *p) {
    std::cout << "Adding reachible peg." << std::endl;
    reachiblePeg = p;
}
