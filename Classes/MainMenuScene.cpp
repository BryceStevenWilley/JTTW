#include "MainMenuScene.hpp"
#include "HelloWorldScene.h"
#include "LevelSelectScene.hpp"
#include <iostream>

using namespace JTTW;

cocos2d::Scene* MainMenu::createScene() {
    // 'scene and layer are auto-release.
    auto scene = cocos2d::Scene::create();
    auto layer = MainMenu::create();
    scene->addChild(layer);
    
    return scene;
}

bool MainMenu::init() {
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();

    // Sets the background image to fill the screen.
    auto background = cocos2d::Sprite::create("backgrounds/Splash.png");
    background->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    background->setPosition(origin);
    auto bgSize = background->getContentSize();
    background->setScale(visibleSize.width / bgSize.width, visibleSize.height / bgSize.height);
    this->addChild(background, -8);
    
    // Set the Font on the screen. (TODO: make it fly in?)
    auto titleImage = cocos2d::Sprite::create("UI/MenuTitle.png");

    titleImage->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    
    float middleX = origin.x + visibleSize.width / 2.0;
    titleImage->setPosition(middleX, origin.y + visibleSize.height * (3.0 / 4.0));
    cocos2d::Size desiredSize = cocos2d::Size(visibleSize.width / 3.0, visibleSize.height / 6.0);
    titleImage->setScale(desiredSize.width / titleImage->getContentSize().width, desiredSize.height / titleImage->getContentSize().height);
    
    auto playImage = cocos2d::Sprite::create("UI/MenuPlay.png");
    auto settingsImage = cocos2d::Sprite::create("UI/MenuSettings.png");
    auto exitImage = cocos2d::Sprite::create("UI/MenuExit.png");
    
    playImage->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    settingsImage->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    exitImage->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    
    auto playImageS = cocos2d::Sprite::create("UI/MenuPlay.png");
    auto settingsImageS = cocos2d::Sprite::create("UI/MenuSettings.png");
    auto exitImageS = cocos2d::Sprite::create("UI/MenuExit.png");
    playImageS->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    settingsImageS->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    exitImageS->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    playImageS->setScale(1.3);
    settingsImageS->setScale(1.3);
    exitImageS->setScale(1.3);
    
    cocos2d::MenuItemSprite *playItem = cocos2d::MenuItemSprite::create(playImage, playImageS, CC_CALLBACK_0(MainMenu::openStartScene, this));
    cocos2d::MenuItemSprite *settingsItem = cocos2d::MenuItemSprite::create(settingsImage, settingsImageS, settingsImage, CC_CALLBACK_0(MainMenu::openSettings, this));
    cocos2d::MenuItemSprite *exitItem = cocos2d::MenuItemSprite::create(exitImage, exitImageS, CC_CALLBACK_0(MainMenu::exitGame, this));
    
    desiredSize = desiredSize * 0.6;
    playItem->setScale(desiredSize.width / playItem->getContentSize().width, desiredSize.height / playItem->getContentSize().height);
    exitItem->setScale(desiredSize.width / exitItem->getContentSize().width, desiredSize.height / exitItem->getContentSize().height);
    desiredSize.width *= 1.4;
    settingsItem->setScale(desiredSize.width / settingsItem->getContentSize().width, desiredSize.height / settingsItem->getContentSize().height);

    playItem->setPosition(middleX, origin.y + visibleSize.height / 2.0);
    settingsItem->setPosition(middleX, origin.y + visibleSize.height * (2.0 / 6.0));
    exitItem->setPosition(middleX, origin.y + visibleSize.height * (1.0 / 6.0));
    
    // TODO: right now, the menu only changes to selected when clicked.
    // Make it so that the first starts being selected, can navigate with arrow keys, or if the mouse starts moving, it selects
    // whichever it intersects with.
    cocos2d::Menu *m = cocos2d::Menu::create(playItem, settingsItem, exitItem, NULL);
    m->setPosition(0,0);
    
    this->addChild(titleImage, -6);
    this->addChild(m, -6);
    
    auto eventListener = cocos2d::EventListenerKeyboard::create();
    eventListener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
        if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_ENTER) {
            this->openStartScene();
        }
    };
    this->_eventDispatcher->addEventListenerWithFixedPriority(eventListener, 1);
    
    return true;
}

void MainMenu::openStartScene() {
    auto startScene = LevelSelect::createScene();
    cocos2d::Director::getInstance()->replaceScene(startScene);
}

void MainMenu::openSettings() {
    std::cout << "Opened settings!" << std::endl;
    // TODO: Actually open settings.
}

void MainMenu::exitGame() {
    // Close the game and quit the application
    cocos2d::Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        exit(0);
    #endif
}

