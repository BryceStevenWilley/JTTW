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
        LevelInfoStruct info;
        info.path = ss1.str();
        info.fileName = toReturn[i];
        allLevels.push_back(info);
    }
    for (size_t i = 0; i < allLevels.size(); i++) {
        std::ifstream inFile(allLevels[i].path);
        nlohmann::json lvl;
        inFile >> lvl;
    
        cocos2d::Sprite *img;
        try {
            std::string levelName = lvl["levelName"];
            allLevels[i].title = levelName;
            std::stringstream ss;
            ss << "levelFiles/previews/" << allLevels[i].title << "Preview.png";
            std::string imgPath = ss.str();
            allLevels[i].order = lvl["levelNumber"];
            img = cocos2d::Sprite::create(imgPath);
            if (img == NULL) {
                std::cout << "couldn't find image file for " << imgPath << std::endl;
                img = cocos2d::Sprite::create("assets/lock.png");
            }
        } catch (std::domain_error ex) {
            std::cout << "Failed to parse " << allLevels[i].path << std::endl;
            allLevels[i].title = allLevels[i].path;
            img = cocos2d::Sprite::create();
        }
        img->setPosition(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (1.0/2.0));
        img->setVisible(false);
        img->setContentSize(visibleSize * (4.5 / 10.0));
        allLevels[i].thumbnail = img;
       
        this->addChild(img);
    }
    
    std::sort(allLevels.begin(), allLevels.end());
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
    instructions->setPosition(origin.x + visibleSize.width / 8.0, origin.y + visibleSize.height * (2.0/ 13.0));
    this->addChild(instructions);

    // Create menu items for each of the level files that we have.
    findLevelFiles(false);
    if (allLevels.size() == 0) {
        std::cout << "Can't find any level files!" << std::endl;
        return false;
    }

    currentLevel = 0;
    allLevels[currentLevel].thumbnail->setVisible(true);
    
    auto border = cocos2d::Sprite::create("levelFiles/previews/Border.png");
    border->setPosition(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (1.0/2.0));
    border->setContentSize(visibleSize * (6.0 / 10.0));
    border->setScaleX(border->getScaleX() * .92);
    this->addChild(border, 10);
    
    levelName = createSunriseLabel(allLevels[currentLevel].title, 60, fontScaleFactor);
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
                allLevels[currentLevel].thumbnail->setVisible(false);
                if (currentLevel == 0) {
                    currentLevel = (int)allLevels.size();
                }
                currentLevel--;
                levelName->setString(allLevels[currentLevel].title);
                allLevels[currentLevel].thumbnail->setVisible(true);
                break;
            
            case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
                allLevels[currentLevel].thumbnail->setVisible(false);
                currentLevel++;
                if (currentLevel == allLevels.size()) {
                    currentLevel = 0;
                }
                levelName->setString(allLevels[currentLevel].title);
                allLevels[currentLevel].thumbnail->setVisible(true);
                break;
            
            case cocos2d::EventKeyboard::KeyCode::KEY_ENTER:
            case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
                this->menuCallback(allLevels[currentLevel].path);
                break;
                
            case cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE: {
                this->_eventDispatcher->removeEventListener(keyListener);
                auto mainmenu = MainMenu::createScene();
                cocos2d::Director::getInstance()->replaceScene(mainmenu);
                return;
            }
            
            case cocos2d::EventKeyboard::KeyCode::KEY_9:
                for (size_t i = 0; i < allLevels.size(); i++) {
                    this->removeChild(allLevels[i].thumbnail);
                }
            
                // Activates debug mode!
                allLevels.clear();
                
                findLevelFiles(true);
                currentLevel = 0;
                levelName->setString(allLevels[currentLevel].title);
                allLevels[currentLevel].thumbnail->setVisible(true);
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
    titleLabel->setString("Loading...");
    
    auto loadLevel = cocos2d::CallFunc::create([this, newLevel]() {
        cocos2d::Scene *startScene = nullptr;
        startScene = MainGameScene::createScene(newLevel);
        if (startScene == nullptr) {
           titleLabel->setString("Something went wrong!\n Choose a different level!");
           return;
        }
        //CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
        this->_eventDispatcher->removeEventListener(keyListener);
        auto fade = cocos2d::TransitionFade::create(1.5, startScene);
        cocos2d::Director::getInstance()->replaceScene(fade);
    });

    this->runAction(cocos2d::Sequence::create(cocos2d::FadeIn::create(.2), loadLevel, NULL));
}
