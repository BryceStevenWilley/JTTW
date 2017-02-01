#include "GameObject.hpp"
#include "Rectangle.hpp"

using namespace JTTW;

GameObject::GameObject(Rectangle dimensions) : dimensions(dimensions), collisionBoxNode(cocos2d::DrawNode::create()) {}


void GameObject::drawCollisionBox() {
    cocos2d::Color4F black(0.0, 1.0, 0.0, 1.0);
    collisionBoxNode->clear();
    collisionBoxNode->drawPolygon(dimensions.getPoints().data(), 4, black, 0, black);
}
