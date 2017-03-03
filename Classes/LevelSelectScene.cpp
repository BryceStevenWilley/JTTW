#include "LevelSelectScene.hpp"
#include "MainGameScene.h"
#include "MainMenuScene.hpp"
#include "cocos-ext.h"
#include <iostream>
#include "json.hpp"

#include <fstream>
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

void LevelSelect::findLevelFiles(bool includeDev) {
    std::vector<std::string> toReturn;
    DIR *currentDir;
    struct dirent *dirEntry;
    
    currentDir = opendir("levelFiles");
    if (currentDir == NULL) {
        std::cout << "Couldn't open level files." << std::endl;
        return;
    }
    
    // Iterater through all the entries in this directory.
    do {
        if ((dirEntry = readdir(currentDir)) != NULL) {
            // If the file ends in ".json", it's a level file.
            if (strcmp(strrchr(dirEntry->d_name, '.'), ".json") == 0) {
                toReturn.push_back(std::string(dirEntry->d_name));
            }
            if (includeDev && strcmp(strrchr(dirEntry->d_name, '.'), ".old") == 0) {
                toReturn.push_back(std::string(dirEntry->d_name));
            }
        }
    } while (dirEntry != NULL);
    
    (void) closedir(currentDir);

    for (int i = 0; i < (int)toReturn.size(); i++) {
        std::stringstream ss1;
        ss1 << "levelFiles/" << toReturn[i];
        allLevelPaths.push_back(ss1.str());
    }
    for (int i = 0; i < (int)allLevelPaths.size(); i++) {
        std::ifstream inFile(allLevelPaths[i]);
        nlohmann::json lvl;
        inFile >> lvl;
    
        try {
            allLevelNames.push_back(lvl["levelName"]);
            std::stringstream ss;
            ss << "levelFiles/previews/" << lvl["levelName"] << "Preview.png";
            std::string imgPath = ss.str();

            cocos2d::Sprite *img = cocos2d::Sprite::create(imgPath);
            if (img != NULL) {
                //allLevelThumbnails.push_back(img);
                //this->addChild(img);
            } else {
                //std::cout << "couldn't find image file for " << imgPath << std::endl;
                //img = cocos2d::Sprite::create();
                //allLevelThumbnails.push_back(img);
            }
        } catch (std::domain_error ex) {
            std::cout << "Couldn't find the level name for " << allLevelPaths[i];
            allLevelNames.push_back(allLevelPaths[i]);
            //cocos2d::Sprite *img = cocos2d::Sprite::create();
            //allLevelThumbnails.push_back(img);
        }
    }
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
    findLevelFiles(false);
    if (allLevelPaths.size() == 0) {
        std::cout << "Can't find any level files!" << std::endl;
        return false;
    }
    currentLevel = 0;
    
    levelName = cocos2d::Label::createWithTTF(allLevelNames[currentLevel], "fonts/WaitingfortheSunrise.ttf", 40);
    
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
    
    devMode = cocos2d::Label::createWithTTF("DEV MODE = ON", "fonts/WaitingfortheSunrise.ttf", 40);
    
    devMode->setPosition(origin.x + visibleSize.width / 4.0, origin.y + visibleSize.height / 4.0);
    devMode->enableShadow();
    devMode->setVisible(false);
    this->addChild(devMode);

    keyListener = cocos2d::EventListenerKeyboard::create();
    keyListener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
        switch(keyCode) {
            case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
                if (currentLevel == 0) {
                    currentLevel = allLevelNames.size();
                }
                currentLevel--;
                levelName->setString(allLevelNames[currentLevel]);
                break;
            
            case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
                currentLevel++;
                if (currentLevel == allLevelNames.size()) {
                    currentLevel = 0;
                }
                levelName->setString(allLevelNames[currentLevel]);
                break;
            
            case cocos2d::EventKeyboard::KeyCode::KEY_ENTER:
                this->menuCallback(allLevelPaths[currentLevel]);
                break;
                
            case cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE: {
                this->_eventDispatcher->removeEventListener(keyListener);
                auto mainmenu = MainMenu::createScene();
                cocos2d::Director::getInstance()->replaceScene(mainmenu);
                return;
            }
            
            case cocos2d::EventKeyboard::KeyCode::KEY_9:
                // Activates debug mode!
                allLevelNames.clear();
                allLevelPaths.clear();
                allLevelThumbnails.clear();
                findLevelFiles(true);
                currentLevel = 0;
                levelName->setString(allLevelNames[currentLevel]);
                devMode->setVisible(true);
                break;
                
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
    startScene = MainGameScene::createScene(newLevel);
    if (startScene == NULL) {
        titleLabel->setString("Something went wrong!\n Choose a different level!");
        titleLabel->setBMFontSize(40);
        return;
    }
    this->_eventDispatcher->removeEventListener(keyListener);
    auto fade = cocos2d::TransitionFade::create(1.5, startScene);
    cocos2d::Director::getInstance()->replaceScene(fade);
}
