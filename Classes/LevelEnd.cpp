//
//  LevelEnd.cpp
//  JTTW
//
//  Created by Bryce Willey on 2/14/17.
//
//

#include "LevelEnd.hpp"
#include "HelloWorldScene.h"
#include "MainMenuScene.hpp"

using namespace JTTW;

cocos2d::Scene* LevelEnd::createScene(std::string &levelToLoad) {
    // 'scene' and layer are autorelease objects.
    auto scene = cocos2d::Scene::create();
    auto layer = LevelEnd::create(levelToLoad);
    scene->addChild(layer);

    return scene;
}

bool LevelEnd::init(std::string &nextLevelToLoad) {
    _nextLevelToLoad = nextLevelToLoad;

    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    
    auto textLabel = cocos2d::Label::createWithTTF("Your journey continues...", "fonts/WaitingfortheSunrise.ttf", 100);
    if (_nextLevelToLoad == "") {
        // The journey is over!
        textLabel = cocos2d::Label::createWithTTF("The journey is over. Monk went on to be the President. Piggy decided to go back and get his G.E.D...", "fonts/WaitingfortheSunrise.ttf", 20);
    }
    
    textLabel->setTextColor(cocos2d::Color4B::WHITE);
    textLabel->enableOutline(cocos2d::Color4B::BLACK);
    textLabel->enableShadow();
    textLabel->setPosition(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (3.0 / 4.0));
    
     this->addChild(textLabel);
    
    keyListener = cocos2d::EventListenerKeyboard::create();
    keyListener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
        menuCallback();
    };
    mouseListener = cocos2d::EventListenerMouse::create();
    mouseListener->onMouseUp = [this](cocos2d::EventMouse *mouseCode) {
        menuCallback();
    };
    
    this->_eventDispatcher->addEventListenerWithFixedPriority(keyListener, 3);
    this->_eventDispatcher->addEventListenerWithFixedPriority(mouseListener, 3);
    
    return true;
}

void LevelEnd::menuCallback() {
    this->_eventDispatcher->removeEventListener(keyListener);
    this->_eventDispatcher->removeEventListener(mouseListener);
    if (_nextLevelToLoad == "") {
        auto mainmenu = MainMenu::createScene();
        cocos2d::Director::getInstance()->replaceScene(mainmenu);
        return;
    }
    std::stringstream ss;
    ss << "levelFiles/" << _nextLevelToLoad;
    
    std::cout << ss.str() << std::endl;
    
    auto startScene = HelloWorld::createScene(ss.str());
    auto fade = cocos2d::TransitionFade::create(5, startScene);
    
    cocos2d::Director::getInstance()->replaceScene(fade);
}

