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

void JTTW::parseLevelFromJson(std::string fileName, cocos2d::Layer *layer, std::vector<GameObject *> &platforms, std::vector<MoveablePlatform *> &movables, Viewpoint vp, bool debugOn) {
    
    const int platformZ = 4;
    
    std::ifstream inFile(fileName);
    nlohmann::json lvl;
    
    std::cout.flush();
    inFile >> lvl;
    nlohmann::json platformAtts = lvl["platforms"];
    for (auto& pAtt: platformAtts) {
        std::string fullImagePath = pAtt["imageName"];
        fullImagePath = "platforms/" + fullImagePath;
        double centerX = vp.metersToPixels((double)pAtt["centerX"]);
        double centerY = vp.metersToPixels((double)pAtt["centerY"]);
        double imageSizeWidth = vp.metersToPixels((double)pAtt["imageSizeWidth"]);
        double imageSizeHeight = vp.metersToPixels((double)pAtt["imageSizeHeight"]);
        double collisionWidth = vp.metersToPixels((double)pAtt["collisionWidth"]);
        double collisionHeight = vp.metersToPixels((double)pAtt["collisionHeight"]);
        
        if (pAtt["moveable"]) {
            cocos2d::Vec2 centerA(centerX, centerY);
            double centerBX = vp.metersToPixels((double)pAtt["center2X"]);
            double centerBY = vp.metersToPixels((double)pAtt["center2Y"]);
            cocos2d::Vec2 centerB(centerBX, centerBY);
            double maximumVelocity = vp.metersToPixels((double)pAtt["maximumVelocity"]);
            MoveablePlatform *p = new MoveablePlatform(fullImagePath, centerA, centerB, cocos2d::Size(imageSizeWidth, imageSizeHeight), cocos2d::Vec2(collisionWidth, collisionHeight), maximumVelocity);
            auto physics = cocos2d::PhysicsBody::createBox(cocos2d::Size(collisionWidth, collisionHeight));
            physics->setDynamic(false); // moving platforms are kinematic bodies.
            physics->setTag(1);
            physics->setContactTestBitmask(0xFFFFFFFF);
            p->image->addComponent(physics);
            
            layer->addChild(p->image, platformZ);
            platforms.push_back(p);
            movables.push_back(p);
        } else {
            Platform *p = new Platform(fullImagePath, cocos2d::Vec2(centerX, centerY), cocos2d::Size(imageSizeWidth, imageSizeHeight), cocos2d::Vec2(collisionWidth, collisionHeight));
        
            auto physics = cocos2d::PhysicsBody::createBox(cocos2d::Size(collisionWidth, collisionHeight));
            physics->setDynamic(false); // moving platforms are kinematic bodies.
            physics->setTag(1);
             physics->setContactTestBitmask(0xFFFFFFFF);
            p->getImage()->addComponent(physics);
            
            layer->addChild(p->getImage(), platformZ);
            platforms.push_back(p);

            /*
            if (debugOn) {
                auto rectNode = cocos2d::DrawNode::create();
                cocos2d::Color4F black(0.0, 1.0, 1.0, 1.0);
                std::array<cocos2d::Vec2, 4> rect = p->getCollisionBounds().getPoints();
                rectNode->drawPolygon(rect.data(), 4, black, 0, black);
                layer->addChild(rectNode, 3);
                rectNode->clear();
                rectNode->drawPolygon(rect.data(), 4, black, 0, black);
            }
            */
        }
    }
}
