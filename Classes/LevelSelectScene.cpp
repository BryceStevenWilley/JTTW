//
//  LevelSelectScene.cpp
//  JTTW
//
//  Created by Bryce Willey on 2/4/17.
//
//

#include "LevelSelectScene.hpp"
#include "HelloWorldScene.h"
#include <iostream>

using namespace JTTW;

const int DEMO_LEVEL_TAG = 10;

cocos2d::Scene* LevelSelect::createScene() {
    // 'scene' and 'layer' are auto-release.
    auto scene = cocos2d::Scene::create();
    auto layer = LevelSelect::create();
    scene->addChild(layer);
    
    return scene;
}

bool LevelSelect::init() {
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();

    // Choose level label.
    auto chooseLabel = cocos2d::Label::createWithTTF("Choose a level!", "fonts/arial.ttf", 24);
    chooseLabel->setPosition(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (3.0 / 4.0));
    this->addChild(chooseLabel);
    
    // Level labels (hardcoded).
    auto demoLevel = cocos2d::Label::createWithTTF("demoLevel.json", "fonts/arial.ttf", 18);
    cocos2d::MenuItem *demoItem = cocos2d::MenuItemLabel::create(demoLevel, CC_CALLBACK_1(LevelSelect::menuCallback, this));
    demoItem->setTag(DEMO_LEVEL_TAG);
    
    // Display them.
    cocos2d::Menu *m = cocos2d::Menu::create(demoItem, NULL);
    
    this->addChild(m);

    return true;
}

void LevelSelect::menuCallback(cocos2d::Ref* fromItem) {
    auto menuSelection = (cocos2d::MenuItem *) fromItem;
    switch (menuSelection->getTag()) {
        case DEMO_LEVEL_TAG: {
                auto startScene = HelloWorld::createScene();
                cocos2d::Director::getInstance()->replaceScene(startScene);
        }
    }
}
