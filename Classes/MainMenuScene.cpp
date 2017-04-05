#include "MainMenuScene.hpp"
#include "MainGameScene.h"
#include "LevelSelectScene.hpp"
#include <spine/spine-cocos2dx.h>
#include "SimpleAudioEngine.h"
#include <iostream>

using namespace JTTW;

cocos2d::Label * JTTW::createSunriseLabel(std::string content, int size, float scaleFactor) {
    // TODO: we'd perfer to scale the label with setScale, but that changes the positions of things on the main menu. Fix.
    // The current solutions makes things blurry?
    auto label = cocos2d::Label::createWithTTF(content, "fonts/WaitingfortheSunrise.ttf", (int)(size * scaleFactor));
    label->setTextColor(cocos2d::Color4B::WHITE);
    label->enableOutline(cocos2d::Color4B::BLACK, 1);
    label->enableShadow();
    return label;
}

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
    float middleX = origin.x + visibleSize.width / 2.0;

    auto fontScaleFactor = visibleSize.width / 1560.0;

    // Sets the background image to fill the screen.
    auto background = cocos2d::Sprite::create("assets/Splash.png");
    background->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    background->setPosition(origin);
    auto bgSize = background->getContentSize();
    background->setScale(visibleSize.width / bgSize.width, visibleSize.height / bgSize.height);
    this->addChild(background, -8);
    
    // Set the Font on the screen.
    auto titleImage = cocos2d::Sprite::create("UI/MenuTitle.png");
    titleImage->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    cocos2d::Size titleSize = cocos2d::Size(visibleSize.width / 2.0, visibleSize.height / 3.0);
    titleImage->setScale(titleSize.width / titleImage->getContentSize().width, titleSize.height / titleImage->getContentSize().height);

    auto playNode = createSunriseLabel("New Game", 100, fontScaleFactor);
    auto sceneNode = createSunriseLabel("Scene Select", 100, fontScaleFactor);
    auto exitNode = createSunriseLabel("Exit", 100, fontScaleFactor);
    
    cocos2d::MenuItemLabel *playItem = cocos2d::MenuItemLabel::create(playNode, CC_CALLBACK_0(MainMenu::openFirstLevel, this));
    cocos2d::MenuItemLabel *sceneItem = cocos2d::MenuItemLabel::create(sceneNode, CC_CALLBACK_0(MainMenu::openStartScene, this));
    cocos2d::MenuItemLabel *exitItem = cocos2d::MenuItemLabel::create(exitNode, CC_CALLBACK_0(MainMenu::exitGame, this));
    
    options.push_back(playItem);
    options.push_back(sceneItem);
    options.push_back(exitItem);
    currentOption = options.begin();

    playItem->setPosition(middleX, origin.y + visibleSize.height * (4.0/ 9.0));
    sceneItem->setPosition(middleX, origin.y + visibleSize.height * (3.0 / 9.0));
    exitItem->setPosition(middleX, origin.y + visibleSize.height * (2.0/ 9.0));
    
    cocos2d::Menu *m = cocos2d::Menu::create(playItem, sceneItem, exitItem, NULL);
    m->setPosition(0,0);

    this->addChild(m, -6);
    
    auto instructions = createSunriseLabel("<Enter>=Select", 60, fontScaleFactor);
    instructions->setPosition(origin.x + visibleSize.width / 8.0, origin.y + visibleSize.height / 13.0);
    this->addChild(instructions);

    auto up = createSunriseLabel("^", 100, fontScaleFactor);
    up->setPosition(middleX, origin.y + visibleSize.height * (4.7 / 9.0));
    this->addChild(up);
        
    auto down = createSunriseLabel("v", 100, fontScaleFactor);
    down->setPosition(middleX, origin.y + visibleSize.height * (1.0 / 9.0));
    this->addChild(down);
    
    (*currentOption)->setColor(SELECTED);
    (*currentOption)->selected();
    
    eventListener = cocos2d::EventListenerKeyboard::create();
    eventListener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
        switch(keyCode) {
            case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
                (*currentOption)->setColor(UNSELECTED);
                (*currentOption)->unselected();
                if (currentOption == options.begin()) {
                    currentOption = options.end();
                }
                currentOption--;
                (*currentOption)->setColor(SELECTED);
                (*currentOption)->selected();
                break;
            
            case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
                (*currentOption)->setColor(UNSELECTED);
                (*currentOption)->unselected();
                currentOption++;
                if (currentOption == options.end()) {
                    currentOption = options.begin();
                }
                (*currentOption)->setColor(SELECTED);
                (*currentOption)->selected();
                break;
            
            case cocos2d::EventKeyboard::KeyCode::KEY_ENTER:
            case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
                (*currentOption)->activate();
                break;
                
            default:
                // do nothing.
                break;
        }

    };
    this->_eventDispatcher->addEventListenerWithFixedPriority(eventListener, 1);
    
    /*Set up idle characters.*/
    cocos2d::Vec2 monkEnd = cocos2d::Vec2(origin.x + (middleX / 2.0), origin.y + visibleSize.height / 8);
    addCharacterAni("Monk", monkEnd - cocos2d::Vec2(middleX, 0.0), monkEnd);
    cocos2d::Vec2 monkeyEnd = cocos2d::Vec2(origin.x + (middleX / 4), origin.y + visibleSize.height / 8);
    addCharacterAni("Monkey", monkeyEnd - cocos2d::Vec2(middleX / 2.0, 0), monkeyEnd);
    cocos2d::Vec2 piggyEnd = cocos2d::Vec2(visibleSize.width - (middleX / 2.0), origin.y + visibleSize.height / 15.0);
    addCharacterAni("Piggy", piggyEnd + cocos2d::Vec2(middleX, 0.0), piggyEnd);
    cocos2d::Vec2 sandyEnd =  cocos2d::Vec2(visibleSize.width - (middleX / 4.0), origin.y + visibleSize.height / 15.0);
    addCharacterAni("Sandy", sandyEnd + cocos2d::Vec2(middleX / 2.0, 0), sandyEnd);
    
    // Animate the titles to move in.
    titleImage->setPosition(middleX, origin.y + visibleSize.height * (5.0 / 4.0));
    auto titleAni = cocos2d::MoveTo::create(3.0, cocos2d::Vec2(middleX, origin.y + visibleSize.height * (3.3 / 4.0)));
    this->addChild(titleImage, -6);
    titleImage->runAction(cocos2d::EaseIn::create(titleAni, 0.5));
    
    up->setCascadeOpacityEnabled(true);
    down->setCascadeOpacityEnabled(true);
    up->setOpacity(0);
    down->setOpacity(0);
    up->runAction(cocos2d::FadeIn::create(5.0));
    down->runAction(cocos2d::FadeIn::create(5.0));
    instructions->setCascadeOpacityEnabled(true);
    instructions->setOpacity(0);
    instructions->runAction(cocos2d::FadeIn::create(5.0));
    
    playItem->setCascadeOpacityEnabled(true);
    sceneItem->setCascadeOpacityEnabled(true);
    exitItem->setCascadeOpacityEnabled(true);
    playItem->setOpacity(0);
    sceneItem->setOpacity(0);
    exitItem->setOpacity(0);
    playItem->runAction(cocos2d::FadeIn::create(5.0));
    sceneItem->runAction(cocos2d::FadeIn::create(5.0));
    exitItem->runAction(cocos2d::FadeIn::create(5.0));

    return true;
}

spine::SkeletonAnimation *MainMenu::addCharacterAni(std::string name, cocos2d::Vec2 startPosition, cocos2d::Vec2 endPosition) {
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    float middleX = origin.x + visibleSize.width / 2.0;

    // .15 is what the scale was when the screen was 512 by 384, so keep that aspect ratio.
    double charScale = .15 * visibleSize.width / 512.0;

    spine::SkeletonAnimation *c = spine::SkeletonAnimation::createWithJsonFile("characters/" + name + ".json", "characters/" + name + ".atlas", 1.0f);
    c->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    c->setPosition(startPosition);
    c->setTimeScale(0.7 * std::abs(startPosition.x - endPosition.x) / (middleX));
    c->setAnimation(0, "walk", true);
    if (startPosition.x > middleX) {
         c->setScale(charScale);
         c->setScaleX(c->getScaleX() * -1);// Mirror the image.
    } else {
         c->setScale(charScale);
    }
    this->addChild(c);
    
    // Animate walking.
        
    auto makeIdle = cocos2d::CallFunc::create([c]() {
        c->setAnimation(0, "idle", true);
    });
    auto seqC = cocos2d::Sequence::create(cocos2d::MoveTo::create(3.0, endPosition), makeIdle, nullptr);
    c->runAction(seqC);
    
    return c;
}

void MainMenu::openStartScene() {
    this->_eventDispatcher->removeEventListener(eventListener);
    auto startScene = LevelSelect::createScene();
    auto fade = cocos2d::TransitionCrossFade::create(.5, startScene);
    cocos2d::Director::getInstance()->replaceScene(fade);
}

void MainMenu::openFirstLevel() {
    cocos2d::Scene *startScene;
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    startScene = MainGameScene::createScene("levelFiles/Paradise.json");
    if (startScene == NULL) {
        //titleLabel->setString("Something went wrong!\n Choose a different level!");
        //titleLabel->setBMFontSize(40);
        std::cout << "WHY CAN'T WE CHOOSE THE FIRST LEVEL?" << std::endl;
        return;
    }
    this->_eventDispatcher->removeEventListener(eventListener);
    auto fade = cocos2d::TransitionFade::create(1.5, startScene);
    cocos2d::Director::getInstance()->replaceScene(fade);
}

void MainMenu::exitGame() {
    std::cout << "Exiting game from main screen." << std::endl;
    // Close the game and quit the application
    cocos2d::Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        exit(0);
    #endif
}

