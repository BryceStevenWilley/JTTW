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
            if (strrchr(dirEntry->d_name, '.') != NULL && strcmp(strrchr(dirEntry->d_name, '.'), ".json") == 0) {
                toReturn.push_back(std::string(dirEntry->d_name));
            }
            if (strrchr(dirEntry->d_name, '.') != NULL && includeDev && strcmp(strrchr(dirEntry->d_name, '.'), ".dev") == 0) {
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
    
        cocos2d::Sprite *img;
        try {
            std::string levelName = lvl["levelName"];
            allLevelNames.push_back(levelName);
            std::stringstream ss;
            ss << "levelFiles/previews/" << levelName << "Preview.png";
            std::string imgPath = ss.str();
            orderedLevels[lvl["levelNumber"]] = levelName;
            img = cocos2d::Sprite::create(imgPath);
            if (img == NULL) {
                std::cout << "couldn't find image file for " << imgPath << std::endl;
                img = cocos2d::Sprite::create("assets/lock.png");
            }
        } catch (std::domain_error ex) {
            std::cout << "Failed to parse " << allLevelPaths[i] << std::endl;
            allLevelNames.push_back(allLevelPaths[i]);
            img = cocos2d::Sprite::create();
        }
        img->setPosition(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (1.0/2.0));
        img->setVisible(false);
        img->setContentSize(visibleSize * (4.5 / 10.0));
        allLevelThumbnails.push_back(img);
       
        this->addChild(img);
    }
}

// Sets up the selection menu by looking through the level folder and displaying
// all .json files.
bool LevelSelect::init() {
    visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    auto fontScaleFactor = visibleSize.width / 1560.0;

    // Sets the background image to fill the screen.
    auto background = cocos2d::Sprite::create("assets/Splash.png");
    background->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    background->setPosition(origin);
    auto bgSize = background->getContentSize();
    background->setScale(visibleSize.width / bgSize.width, visibleSize.height / bgSize.height);
    this->addChild(background, -8);
    
    // Set the title text as a label.
    titleLabel = createSunriseLabel("Choose a level!", 100, fontScaleFactor);
    titleLabel->setPosition(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (6.0 / 7.0));
    this->addChild(titleLabel);

    auto instructions = createSunriseLabel("<Enter>=Select\n<Esc>=Exit", 60, fontScaleFactor);
    instructions->setPosition(origin.x + visibleSize.width / 8.0, origin.y + visibleSize.height / 13.0);
    this->addChild(instructions);

    // Create menu items for each of the level files that we have.
    findLevelFiles(false);
    if (allLevelPaths.size() == 0) {
        std::cout << "Can't find any level files!" << std::endl;
        return false;
    }

    currentLevel = 0;
    allLevelThumbnails[currentLevel]->setVisible(true);
    
    auto border = cocos2d::Sprite::create("levelFiles/previews/Border.png");
    border->setPosition(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (1.0/2.0));
    border->setContentSize(visibleSize * (6.0 / 10.0));
    border->setScaleX(border->getScaleX() * .92);
    this->addChild(border, 10);
    
    levelName = createSunriseLabel(allLevelNames[currentLevel], 60, fontScaleFactor);
    levelName->setPosition(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (1.0/7.0));
    this->addChild(levelName);
    
    auto leftArrow = createSunriseLabel("<", 100, fontScaleFactor);
    auto rightArrow = createSunriseLabel(">", 100, fontScaleFactor);
    leftArrow->setPosition(origin.x + visibleSize.width * (1.0 /6.0), origin.y + visibleSize.height / 2.0);
    rightArrow->setPosition(origin.x + visibleSize.width * (5.0/6.0), origin.y + visibleSize.height / 2.0);
    this->addChild(leftArrow);
    this->addChild(rightArrow);
    
    devMode = createSunriseLabel("DEV MODE = ON", 40, fontScaleFactor);
    devMode->setPosition(origin.x + visibleSize.width / 4.0, origin.y + visibleSize.height / 4.0);
    devMode->setVisible(false);
    this->addChild(devMode, 11);

    keyListener = cocos2d::EventListenerKeyboard::create();
    keyListener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
        switch(keyCode) {
            case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
                allLevelThumbnails[currentLevel]->setVisible(false);
                if (currentLevel == 0) {
                    currentLevel = (int)allLevelNames.size();
                }
                currentLevel--;
                levelName->setString(allLevelNames[currentLevel]);
                allLevelThumbnails[currentLevel]->setVisible(true);
                break;
            
            case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
                allLevelThumbnails[currentLevel]->setVisible(false);
                currentLevel++;
                if (currentLevel == allLevelNames.size()) {
                    currentLevel = 0;
                }
                levelName->setString(allLevelNames[currentLevel]);
                allLevelThumbnails[currentLevel]->setVisible(true);
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
                for (int i = 0; i < (int)allLevelThumbnails.size(); i++) {
                    this->removeChild(allLevelThumbnails[i]);
                }
            
                // Activates debug mode!
                allLevelNames.clear();
                allLevelPaths.clear();
                allLevelThumbnails.clear();
                
                findLevelFiles(true);
                currentLevel = 0;
                levelName->setString(allLevelNames[currentLevel]);
                allLevelThumbnails[currentLevel]->setVisible(true);
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
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    startScene = MainGameScene::createScene(newLevel);
    //CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    if (startScene == NULL) {
        titleLabel->setString("Something went wrong!\n Choose a different level!");
        //titleLabel->setBMFontSize(40);
        return;
    }
    this->_eventDispatcher->removeEventListener(keyListener);
    auto fade = cocos2d::TransitionFade::create(1.5, startScene);
    cocos2d::Director::getInstance()->replaceScene(fade);
}
