//
//  LevelSelectScene.cpp
//  JTTW
//
//  Created by Bryce Willey on 2/4/17.
//
//

#include "LevelSelectScene.hpp"

using namespace JTTW;

cocos2d::Scene* LevelSelect::createScene() {
    // 'scene' and 'layer' are auto-release.
    auto scene = cocos2d::Scene::create();
    auto layer = LevelSelect::create();
    scene->addChild(layer);
    
    return scene;
}

bool LevelSelect::init() {

    return true;
}
