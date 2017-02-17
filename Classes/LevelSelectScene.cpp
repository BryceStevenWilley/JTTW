#include "LevelSelectScene.hpp"
#include "MainGameScene.h"
#include "MainMenuScene.hpp"
#include "cocos-ext.h"
#include <iostream>

#include <stdio.h>
#include <string.h>
#include <dirent.h>

using namespace JTTW;

cocos2d::Scene* LevelSelect::createScene() {
    // 'scene' and 'layer' are auto-release.
    auto scene = cocos2d::Scene::create();
    auto layer = LevelSelect::create();
    scene->addChild(layer);
    
    return scene;
}

std::vector<std::string> findLevelFiles() {
    std::vector<std::string> toReturn;
    DIR *currentDir;
    struct dirent *dirEntry;
    
    currentDir = opendir("levelFiles");
    if (currentDir == NULL) {
        std::cout << "Couldn't open level files." << std::endl;
        return toReturn;
    }
    
    // Iterater through all the entries in this directory.
    do {
        if ((dirEntry = readdir(currentDir)) != NULL) {
            // If the file ends in ".json", it's a level file.
            if (strcmp(strrchr(dirEntry->d_name, '.'), ".json") == 0) {
                toReturn.push_back(std::string(dirEntry->d_name));
            }
        }
    } while (dirEntry != NULL);
    
    (void) closedir(currentDir);
    return toReturn;
}

// Sets up the selection menu by looking through the level folder and displaying
// all .json files.
bool LevelSelect::init() {
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();

    // Sets the background image to fill the screen.
    auto background = cocos2d::Sprite::create("backgrounds/Splash.png");
    background->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    background->setPosition(origin);
    auto bgSize = background->getContentSize();
    background->setScale(visibleSize.width / bgSize.width, visibleSize.height / bgSize.height);
    this->addChild(background, -8);
    
    // Set the title text as a label.
    titleLabel = cocos2d::Label::createWithTTF("Choose a level!", "fonts/WaitingfortheSunrise.ttf", 100);
    titleLabel->setTextColor(cocos2d::Color4B::WHITE);
    titleLabel->enableOutline(cocos2d::Color4B::BLACK);
    titleLabel->enableShadow();
    titleLabel->setPosition(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (3.0 / 4.0));
    this->addChild(titleLabel);

    // Create menu items for each of the level files that we have.
    // TODO: This won't work well for many level files (they'll fall off the bottom).
    allLevels = findLevelFiles();
    currentLevel = allLevels.begin();
    
    levelName = cocos2d::Label::createWithTTF(*currentLevel, "fonts/WaitingfortheSunrise.ttf", 40);
    
    levelName->setPosition(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height / 2.0);
    levelName->enableShadow();
    
    this->addChild(levelName);
    
    auto leftArrow = cocos2d::Label::createWithTTF("<", "fonts/WaitingfortheSunrise.ttf", 100);
    auto rightArrow = cocos2d::Label::createWithTTF(">", "fonts/WaitingfortheSunrise.ttf", 100);
    leftArrow->setTextColor(cocos2d::Color4B::WHITE);
    leftArrow->enableOutline(cocos2d::Color4B::BLACK);
    leftArrow->enableShadow();
    rightArrow->setTextColor(cocos2d::Color4B::WHITE);
    rightArrow->enableOutline(cocos2d::Color4B::BLACK);
    rightArrow->enableShadow();
    leftArrow->setPosition(origin.x + visibleSize.width / 4.0, origin.y + visibleSize.height / 2.0);
    rightArrow->setPosition(origin.x + visibleSize.width * (3.0/4.0), origin.y + visibleSize.height / 2.0);
    
    this->addChild(leftArrow);
    this->addChild(rightArrow);

    keyListener = cocos2d::EventListenerKeyboard::create();
    keyListener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
        switch(keyCode) {
            case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
                if (currentLevel == allLevels.begin()) {
                    currentLevel = allLevels.end();
                }
                currentLevel--;
                levelName->setString(*currentLevel);
                break;
            
            case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
                currentLevel++;
                if (currentLevel == allLevels.end()) {
                    currentLevel = allLevels.begin();
                }
                levelName->setString(*currentLevel);
                break;
            
            case cocos2d::EventKeyboard::KeyCode::KEY_ENTER:
                this->menuCallback(*currentLevel);
                break;
                
            case cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE: {
                this->_eventDispatcher->removeEventListener(keyListener);
                auto mainmenu = MainMenu::createScene();
                cocos2d::Director::getInstance()->replaceScene(mainmenu);
                return;
            }
                
            default:
                // do nothing.
                break;
        }
    };
    
    this->_eventDispatcher->addEventListenerWithFixedPriority(keyListener, 3);
    return true;
}

// Go to whatever level was selected.
void LevelSelect::menuCallback(std::string newLevel) {
    cocos2d::Scene *startScene;
    std::stringstream ss;
    ss << "levelFiles/" << newLevel;
    startScene = HelloWorld::createScene(ss.str());
    if (startScene == NULL) {
        titleLabel->setString("Something went wrong!\n Choose a different level!");
        titleLabel->setBMFontSize(40);
        return;
    }
    this->_eventDispatcher->removeEventListener(keyListener);
    auto fade = cocos2d::TransitionFade::create(1.5, startScene);
    cocos2d::Director::getInstance()->replaceScene(fade);
}
