#ifndef Cutscene_hpp
#define Cutscene_hpp

#include <iostream>
#include "MainGameScene.h"
#include "Zone.hpp"
#include "cocos2d.h"

namespace JTTW {

const int DRAGON_CUTSCENE = 1;


class Cutscene {
public:
    Cutscene(int uid, Viewpoint vp, Zone z);

    void initFromScene(MainGameScene *scene);
    void runScene(bool gotoStartBlackout);
    
    bool trigger();
private:
    std::map<std::string, cocos2d::Vec2> charactersStart;
    std::map<std::string, cocos2d::Vec2> charactersEnd;
    
    std::vector<std::string> triggeringCharacters;
    
    // Initiliazed from MainSceneObject.
    std::map<std::string, Character *> _characters;
    MainGameScene *_scene;
    cocos2d::Sprite *_curtain;
    cocos2d::EventListenerKeyboard *_listener;
    cocos2d::EventListenerKeyboard *_keySync;
    std::map<cocos2d::EventKeyboard::KeyCode, bool> _haveReleased;
    cocos2d::EventDispatcher *_eventDispatcher;
    
    Zone _triggerZone;
};
}

#endif 

/* Cutscene_hpp */

