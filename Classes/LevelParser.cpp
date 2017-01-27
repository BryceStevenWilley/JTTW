//
//  LevelParser.cpp
//  JTTW
//
//  Created by Bryce Willey on 1/27/17.
//
//

#include "LevelParser.hpp"
#include <iostream>
#include <fstream>

using namespace JTTW; 

void JTTW::parseLevelFromJson(std::string fileName, cocos2d::Layer *layer, std::vector<Platform> &platforms, Viewpoint vp) {
    
    const int platformZ = 3;
    
    std::ifstream inFile(fileName);
    nlohmann::json lvl;

    std::cout << "Printed well?" << std::endl;
    
    //std::cout << inFile.rdbuf();
    std::cout.flush();
    inFile >> lvl;
    
    nlohmann::json platformAtts = lvl["platforms"];
    for (auto& pAtt: platformAtts) {
        std::string fullImagePath = pAtt["imageName"];
        fullImagePath = "platforms/" + fullImagePath;
        double centerX = vp.metersToPixels(pAtt["centerX"]);
        double centerY = vp.metersToPixels(pAtt["centerY"]);
        double imageSizeWidth = vp.metersToPixels(pAtt["imageSizeWidth"]);
        double imageSizeHeight = vp.metersToPixels(pAtt["imageSizeHeight"]);
        double collisionWidth = vp.metersToPixels(pAtt["collisionWidth"]);
        double collisionHeight = vp.metersToPixels(pAtt["collisionHeight"]);
        
        Platform p(fullImagePath, cocos2d::Vec2(centerX, centerY), cocos2d::Size(imageSizeWidth, imageSizeHeight), cocos2d::Vec2(collisionWidth, collisionHeight));
        
        //layer->addChild(p.getImage(), platformZ);
        
        auto rectNode = cocos2d::DrawNode::create();
        cocos2d::Color4F black(0.0, 1.0, 1.0, 1.0);
        cocos2d::Vec2 rect[4];
        rect[0] = cocos2d::Vec2(centerX - collisionWidth / 2.0, centerY - collisionHeight / 2.0);
        rect[1] = cocos2d::Vec2(centerX + collisionWidth / 2.0, centerY - collisionHeight / 2.0);
        rect[2] = cocos2d::Vec2(centerX + collisionWidth / 2.0, centerY + collisionHeight / 2.0);
        rect[3] = cocos2d::Vec2(centerX - collisionWidth / 2.0, centerY + collisionHeight / 2.0);
        rectNode->drawPolygon(rect, 4, black, 10, black);
        layer->addChild(rectNode, 7);
        rectNode->clear();
        //rect[1] = cocos2d::Vec2(centerX - collisionWidth / 2.0, centerY - collisionHeight / 2.0);
        //rect[2] = cocos2d::Vec2(centerX + collisionWidth / 2.0, centerY - collisionHeight / 2.0);
        //rect[3] = cocos2d::Vec2(centerX + collisionWidth / 2.0, centerY + collisionHeight / 2.0);
        //rect[4] = cocos2d::Vec2(centerX - collisionWidth / 2.0, centerY + collisionHeight / 2.0);
        rectNode->drawPolygon(rect, 4, black, 10, black);
        //layer->addChild(rectNode, 8);
        
        platforms.push_back(p);
    }
}
