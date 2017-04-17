#ifndef Cutscene_hpp
#define Cutscene_hpp

#include <iostream>
#include "MainGameScene.h"
#include "Zone.hpp"
#include "cocos2d.h"

namespace JTTW {

class Cutscene {
public:
    enum Scene {
        DRAGON,
        FLIGHT,
        BODHI
    };

    Cutscene(Scene uid, Viewpoint vp);

    void initFromScene(MainGameScene *scene);
    virtual void runScene(bool gotoStartBlackout);
    
    virtual bool trigger();
protected:
    cocos2d::ActionInterval *pauseFor(double duration) {
        return cocos2d::ScaleBy::create(duration, 1.0);
    }

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
    
    Scene which;
};
}

#endif 

/* Cutscene_hpp */

