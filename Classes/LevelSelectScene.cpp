//
//  LevelSelectScene.cpp
//  JTTW
//
//  Created by Bryce Willey on 2/4/17.
//
//

#include "LevelSelectScene.hpp"
#include "MainGameScene.h"
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
    auto titleLabel = cocos2d::Label::createWithTTF("Choose a level!", "fonts/WaitingfortheSunrise.ttf", 100);
    titleLabel->setTextColor(cocos2d::Color4B::WHITE);
    titleLabel->enableOutline(cocos2d::Color4B::BLACK);
    titleLabel->enableShadow();
    titleLabel->setPosition(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (3.0 / 4.0));
    this->addChild(titleLabel);

    // Create menu items for each of the level files that we have.
    // TODO: This won't work well for many level files (they'll fall off the bottom.
    std::vector<std::string> allLevels = findLevelFiles();
    cocos2d::Vector<cocos2d::MenuItem *> menuButtons;
    int levelHash = 0;
    for (auto level : allLevels) {
        auto levelName = cocos2d::Label::createWithTTF(level, "fonts/WaitingfortheSunrise.ttf", 40);
        levelName->enableShadow();
        cocos2d::MenuItem *levelItem = cocos2d::MenuItemLabel::create(levelName, CC_CALLBACK_1(LevelSelect::menuCallback, this));
        levelItem->setTag(levelHash);
        menuButtons.pushBack(levelItem);
        
        tagToFileName[levelHash] = level;
        levelHash += 1;
    }

    // Display and activate the menu items.
    cocos2d::Menu *m = cocos2d::Menu::createWithArray(menuButtons);
    m->alignItemsVerticallyWithPadding(40);
    this->addChild(m);

    return true;
}

// Go to whatever level was selected.
void LevelSelect::menuCallback(cocos2d::Ref* fromItem) {
    auto menuSelection = (cocos2d::ui::Button *) fromItem;
    cocos2d::Scene *startScene;
    std::stringstream ss;
    ss << "levelFiles/" << tagToFileName[menuSelection->getTag()];
    startScene = HelloWorld::createScene(ss.str());
    auto fade = cocos2d::TransitionFade::create(1.5, startScene);
    cocos2d::Director::getInstance()->replaceScene(fade);
}
