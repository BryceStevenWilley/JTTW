#include "MainMenuScene.hpp"
#include "LevelSelectScene.hpp"
#include <spine/spine-cocos2dx.h>
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
    titleImage->setPosition(middleX, origin.y + visibleSize.height * (3.3 / 4.0));
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
    
    /*Set up idle characters.*/
    addCharacterAni("Monk", cocos2d::Vec2(origin.x + (middleX/2), origin.y + visibleSize.height / 8));
    addCharacterAni("Monkey", cocos2d::Vec2(origin.x + (middleX/4), origin.y + visibleSize.height / 8));
    addCharacterAni("Piggy", cocos2d::Vec2(visibleSize.width - (middleX/2.0), origin.y + visibleSize.height / 15.0));
    addCharacterAni("Sandy", cocos2d::Vec2(visibleSize.width - (middleX/4.0), origin.y + visibleSize.height / 15.0));
    
    //cocos2d::MenuItemLabel *playItem = cocos2d::MenuItemLabel::create( CC_CALLBACK_);
    cocos2d::MenuItemSprite *playItem = cocos2d::MenuItemSprite::create(playImage, playImageS, CC_CALLBACK_0(MainMenu::openStartScene, this));
    cocos2d::MenuItemSprite *settingsItem = cocos2d::MenuItemSprite::create(settingsImage, settingsImageS, settingsImage, CC_CALLBACK_0(MainMenu::openSettings, this));
    cocos2d::MenuItemSprite *exitItem = cocos2d::MenuItemSprite::create(exitImage, exitImageS, CC_CALLBACK_0(MainMenu::exitGame, this));
    
    options.push_back(playItem);
    options.push_back(settingsItem);
    options.push_back(exitItem);
    currentOption = options.begin();
    
    cocos2d::Size menuItemSize = titleSize * 0.4;
    playItem->setScale(menuItemSize.width / playItem->getContentSize().width, menuItemSize.height / playItem->getContentSize().height);
    exitItem->setScale(menuItemSize.width / exitItem->getContentSize().width, menuItemSize.height / exitItem->getContentSize().height);
    menuItemSize.width *= 2.5;
    menuItemSize.height *= 1.5;
    settingsItem->setScale(menuItemSize.width / settingsItem->getContentSize().width, menuItemSize.height / settingsItem->getContentSize().height);

    playItem->setPosition(middleX + 20, origin.y + visibleSize.height / 2.0);
    settingsItem->setPosition(middleX + 20, origin.y + visibleSize.height / 3.0);
    exitItem->setPosition(middleX + 20, origin.y + visibleSize.height / 5.0);
    
    cocos2d::Menu *m = cocos2d::Menu::create(playItem, settingsItem, exitItem, NULL);
    m->setPosition(0,0);

    this->addChild(titleImage, -6);
    this->addChild(m, -6);

    auto instructions = cocos2d::Label::createWithTTF("<Enter>=Select\n<Esc>=Exit", "fonts/WaitingfortheSunrise.ttf", 60);
    instructions->setTextColor(cocos2d::Color4B::WHITE);
    instructions->enableOutline(cocos2d::Color4B::BLACK);
    instructions->enableShadow();
    instructions->setPosition(origin.x + visibleSize.width / 8.0, origin.y + visibleSize.height / 13.0);
    this->addChild(instructions);

    auto up = cocos2d::Label::createWithTTF("^", "fonts/WaitingfortheSunrise.ttf", 100);
    up->setTextColor(cocos2d::Color4B::WHITE);
    up->enableOutline(cocos2d::Color4B::BLACK);
    up->enableShadow();
    up->setPosition(middleX, origin.y + visibleSize.height * (1.0 / 1.7));
    this->addChild(up);
        
    auto down = cocos2d::Label::createWithTTF("v", "fonts/WaitingfortheSunrise.ttf", 100);
    down->setTextColor(cocos2d::Color4B::WHITE);
    down->enableOutline(cocos2d::Color4B::BLACK);
    down->enableShadow();
    down->setPosition(middleX, origin.y + visibleSize.height * (1.0 / 9.0));
    this->addChild(down);
    
    (*currentOption)->selected();
    
    eventListener = cocos2d::EventListenerKeyboard::create();
    eventListener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
        switch(keyCode) {
            case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
                (*currentOption)->unselected();
                if (currentOption == options.begin()) {
                    currentOption = options.end();
                }
                currentOption--;
                (*currentOption)->selected();
                break;
            
            case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
                (*currentOption)->unselected();
                currentOption++;
                if (currentOption == options.end()) {
                    currentOption = options.begin();
                }
                (*currentOption)->selected();
                break;
            
            case cocos2d::EventKeyboard::KeyCode::KEY_ENTER:
                (*currentOption)->activate();
                break;
                
            case cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE: {
                this->exitGame();
            }
                
            default:
                // do nothing.
                break;
        }

    };
    this->_eventDispatcher->addEventListenerWithFixedPriority(eventListener, 1);
    
    return true;
}

void MainMenu::addCharacterAni(std::string name, cocos2d::Vec2 position) {
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    float middleX = origin.x + visibleSize.width / 2.0;

    // .15 is what the scale was when the screen was 512 by 384, so keep that aspect ratio.
    double charScale = .15 * visibleSize.width / 512.0;

    spine::SkeletonAnimation *c = spine::SkeletonAnimation::createWithJsonFile("characters/" + name + ".json", "characters/" + name + ".atlas", 1.0f);
    c->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    c->setPosition(position);
    c->setAnimation(0, "idle", true);
    if (position.x > middleX) {
         c->setScale(charScale);
         c->setScaleX(c->getScaleX() * -1);// Mirror the image.
    } else {
         c->setScale(charScale);
    }
    this->addChild(c);
}

void MainMenu::openStartScene() {
    this->_eventDispatcher->removeEventListener(eventListener);
    auto startScene = LevelSelect::createScene();
    auto fade = cocos2d::TransitionCrossFade::create(.5, startScene);
    cocos2d::Director::getInstance()->replaceScene(fade);
}

void MainMenu::openSettings() {
    std::cout << "Opened settings!" << std::endl;
    // TODO: Actually open settings.
}

void MainMenu::exitGame() {
    std::cout << "Exiting game from main screen." << std::endl;
    // Close the game and quit the application
    cocos2d::Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        exit(0);
    #endif
}

