#include "LevelEnd.hpp"
#include "MainGameScene.h"
#include "MainMenuScene.hpp"
#include "SimpleAudioEngine.h"

using namespace JTTW;

cocos2d::Scene* LevelEnd::createScene(std::string &levelToLoad, std::string &endQuote) {
    std::cout << "Level end: new level: " << levelToLoad << ", end quote: " << endQuote << std::endl;
    // 'scene' and layer are autorelease objects.
    auto scene = cocos2d::Scene::create();
    auto layer = LevelEnd::create(levelToLoad, endQuote);
    scene->addChild(layer);

    return scene;
}

bool LevelEnd::init(std::string &nextLevelToLoad, std::string &endQuote) {
    _nextLevelToLoad = nextLevelToLoad;
    
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    auto fontScaleFactor = visibleSize.width / 1560.0;

    auto textLabel = createSunriseLabel(endQuote, 60, fontScaleFactor);
    auto smallLabel = createSunriseLabel("Press any key to continue", 40, fontScaleFactor);
    if (_nextLevelToLoad == "") {
        // The journey is over!
        textLabel = createSunriseLabel("Thanks for Playing!", 50, fontScaleFactor);
        smallLabel = createSunriseLabel("Press any key for menu", 40, fontScaleFactor);
        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("Music/Credits.mp3");
    } else {
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Sound/Gong.wav");
    }
    textLabel->setPosition(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (3.0 / 4.0));
    this->addChild(textLabel);

    smallLabel->setPosition(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (1.0/4.0));
    this->addChild(smallLabel);
    return true;
}

void LevelEnd::onEnterTransitionDidFinish() {
    std::cout << "Actually entering level end scene." << std::endl;
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
}

void LevelEnd::menuCallback() {
    this->_eventDispatcher->removeEventListener(keyListener);
    this->_eventDispatcher->removeEventListener(mouseListener);
    if (_nextLevelToLoad == "") {
        //CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
        auto mainmenu = MainMenu::createScene();
        cocos2d::Director::getInstance()->replaceScene(mainmenu);
        return;
    }
    std::stringstream ss;
    ss << "levelFiles/" << _nextLevelToLoad << ".json";
    
    std::cout << ss.str() << std::endl;
    
    auto startScene = MainGameScene::createScene(ss.str());
    if (startScene == nullptr) {
       std::cout << "There is no next level! Return to main" << std::endl;
       
       auto mainmenu = MainMenu::createScene();
       cocos2d::Director::getInstance()->replaceScene(mainmenu);
       return;
    }
    auto fade = cocos2d::TransitionFade::create(3, startScene);
    
    cocos2d::Director::getInstance()->replaceScene(fade);
}


