//
//  LevelParser.cpp
//  JTTW
//
//  Created by Bryce Willey on 1/27/17.
//
//

#include "LevelParser.hpp"
#include "MoveablePlatform.hpp"
#include <iostream>
#include <fstream>

using namespace JTTW; 

nlohmann::json JTTW::parseLevelFromJson(std::string fileName, cocos2d::Layer *level, cocos2d::Layer *platLayer, std::vector<Platform *> &platforms, std::vector<MoveablePlatform *> &movables, Viewpoint vp, bool debugOn) {
    
    const int platformZ = 4;
    
    std::ifstream inFile(fileName);
    nlohmann::json lvl;
    
    std::cout.flush();
    inFile >> lvl;
    
    // draw and add background
    nlohmann::json backgroundAtts = lvl["background"];
    std::string bgPath = backgroundAtts["imageName"];
    bgPath = "backgrounds/" + bgPath;
    auto background = cocos2d::Sprite::create(bgPath);
    background->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    background->setScale(1.4);
    background->setPosition(0,-300.0);
    level->addChild(background, -8);
    
    nlohmann::json platformAtts = lvl["platforms"];
    for (auto& pAtt: platformAtts) {
        std::string fullImagePath = pAtt["imageName"];
        fullImagePath = "platforms/" + fullImagePath;
        double centerX = vp.metersToPixels((double)pAtt["centerX"]);
        double centerY = vp.metersToPixels((double)pAtt["centerY"]);
        double imageSizeWidth = vp.metersToPixels((double)pAtt["imageSizeWidth"]);
        double imageSizeHeight = vp.metersToPixels((double)pAtt["imageSizeHeight"]);
        
        // Get the collision Width and height.
        double collisionWidth = vp.metersToPixels((double)pAtt["collisionWidth"]);
        double collisionHeight = vp.metersToPixels((double)pAtt["collisionHeight"]);
        
        if (pAtt["moveable"]) {
            cocos2d::Vec2 centerA(centerX, centerY);
            double centerBX = vp.metersToPixels((double)pAtt["center2X"]);
            double centerBY = vp.metersToPixels((double)pAtt["center2Y"]);
            cocos2d::Vec2 centerB(centerBX, centerBY);
            double maximumVelocity = vp.metersToPixels((double)pAtt["maximumVelocity"]);
            MoveablePlatform *p = new MoveablePlatform(fullImagePath, centerA, centerB, cocos2d::Size(imageSizeWidth, imageSizeHeight), cocos2d::Vec2(collisionWidth, collisionHeight), maximumVelocity);

            platLayer->addChild(p->image, platformZ);
            platforms.push_back(p);
            movables.push_back(p);
        } else {
            Platform *p = new Platform(fullImagePath, cocos2d::Vec2(centerX, centerY), cocos2d::Size(imageSizeWidth, imageSizeHeight), cocos2d::Vec2(collisionWidth, collisionHeight));
        
            platLayer->addChild(p->getImage(), platformZ);
            platforms.push_back(p);
        }
    }
    return lvl["charactersStart"];
}
