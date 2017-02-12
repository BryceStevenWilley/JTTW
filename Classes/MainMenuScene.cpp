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
    cocos2d::Size titleSize = cocos2d::Size(visibleSize.width / 2.0, visibleSize.height / 3.0);
    titleImage->setScale(titleSize.width / titleImage->getContentSize().width, titleSize.height / titleImage->getContentSize().height);
    
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
    
    /*Set up idle characters.*/
    
    // Monk.
    spine::SkeletonAnimation *monk = spine::SkeletonAnimation::createWithJsonFile("Monk.json", "Monk.atlas", 1.0f);
    monk->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    monk->setPosition(origin.x + (middleX/2), origin.y + visibleSize.height / 8);
    monk->setAnimation(0, "idle", true);
    monk->setScale(.15);
    this->addChild(monk);
    
    // Monkey.
    spine::SkeletonAnimation *monkey = spine::SkeletonAnimation::createWithJsonFile("Monkey.json", "Monkey.atlas", 1.0f);
    monkey->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    monkey->setPosition(origin.x + (middleX/4), origin.y + visibleSize.height / 8);
    monkey->setAnimation(0, "idle", true);
    monkey->setScale(.15);
    this->addChild(monkey);
    
    // Piggy.
    spine::SkeletonAnimation *piggy = spine::SkeletonAnimation::createWithJsonFile("Piggy.json", "Piggy.atlas", 1.0f);
    piggy->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    piggy->setPosition(visibleSize.width - (middleX/2), origin.y + visibleSize.height / 15);
    piggy->setAnimation(0, "idle", true);
    piggy->setScale(.15);
    // Mirror the image.
    piggy->setScaleX(-1 * piggy->getScaleX());
    this->addChild(piggy);
    
    // Sandy.
    spine::SkeletonAnimation *sandy = spine::SkeletonAnimation::createWithJsonFile("sandy.json", "sandy.atlas", 1.0f);
    sandy->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    sandy->setPosition(visibleSize.width - (middleX/4), origin.y + visibleSize.height / 15);
    sandy->setAnimation(0, "idle", true);
    sandy->setScale(.15);
    // Mirror the image.
    sandy->setScaleX(-1 * sandy->getScaleX());
    this->addChild(sandy);
    
    cocos2d::Size menuItemSize = titleSize * 0.4;
    playItem->setScale(menuItemSize.width / playItem->getContentSize().width, menuItemSize.height / playItem->getContentSize().height);
    exitItem->setScale(menuItemSize.width / exitItem->getContentSize().width, menuItemSize.height / exitItem->getContentSize().height);
    menuItemSize.width *= 2.5;
    menuItemSize.height *= 1.5;
    settingsItem->setScale(menuItemSize.width / settingsItem->getContentSize().width, menuItemSize.height / settingsItem->getContentSize().height);

    playItem->setPosition(middleX + 20, origin.y + visibleSize.height / 2.0);
    settingsItem->setPosition(middleX + 20, origin.y + visibleSize.height / 3.0);
    exitItem->setPosition(middleX + 20, origin.y + visibleSize.height / 5.0);
    
    // TODO: right now, the menu only changes to selected when clicked.
    // Make it so that the first starts being selected, can navigate with arrow keys, or if the mouse starts moving, it selects
    // whichever it intersects with.
    cocos2d::Menu *m = cocos2d::Menu::create(playItem, settingsItem, exitItem, NULL);
    m->setPosition(0,0);
    
    this->addChild(titleImage, -6);
    this->addChild(m, -6);
    
    auto eventListener = cocos2d::EventListenerKeyboard::create();
    eventListener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
        if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE) {
            this->exitGame();
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

