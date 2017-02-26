#ifndef MainGameSceneScene_h
#define MainGameSceneScene_h

#include "cocos2d.h"
#include "Character.hpp"
#include "Platform.hpp"
#include "AiAgent.hpp"
#include "Viewpoint.hpp"
#include "MoveablePlatform.hpp"
#include <spine/spine-cocos2dx.h>
#include "Vine.hpp"
#include "Trap.hpp"
#include "json.hpp"

namespace JTTW {
    
class MainGameScene : public cocos2d::Layer {
public:
    static cocos2d::Scene* createScene(std::string levelToLoad);
    virtual bool init(std::string levelToLoad, cocos2d::PhysicsWorld *w);
    void menuCloseCallback(cocos2d::Ref* pSender);
    void nextLevelCallback();
    void switchToCharacter(int charIndex);
    
    //CREATE_FUNC(MainGameScene);
    static MainGameScene* create(std::string levelToLoad, cocos2d::PhysicsWorld *w){
        MainGameScene *pRet = new MainGameScene();
        if (pRet && pRet->init(levelToLoad, w)) {
            pRet->autorelease();
            return pRet;
        }
        else {
            delete pRet;
            pRet = NULL;
            return NULL;
        }
    }
    
    void update(float) override;
    
    bool onContactBegin(cocos2d::PhysicsContact &contact);
    bool onContactEnd(cocos2d::PhysicsContact &contact);
    bool onContactHandler(cocos2d::PhysicsContact &contact, bool begin);
 
private:
    cocos2d::Layer *parseLevelFromJson(std::string fileName, bool debugOn);

    Viewpoint vp = Viewpoint(cocos2d::Size(1.0, 1.0), 1.0);
    std::vector<Character *> characters = std::vector<Character *>();
    std::vector<AiAgent *> agents = std::vector<AiAgent *>();
    AiAgent *player = nullptr;
    
    std::vector<cocos2d::Sprite *> dynamics = std::vector<cocos2d::Sprite *>();
    std::vector<Trap *> trapsToTrigger = std::vector<Trap *>();
    std::vector<Platform *> platforms = std::vector<Platform *>();
    std::vector<MoveablePlatform *> moveables = std::vector<MoveablePlatform *>();
    std::vector<Vine *> vines = std::vector<Vine *>();
    bool debugOn = true; // currently, will just turn on collision boxes.

    bool nextLevelStarting = false;
    bool afterFirstClick = false;

    double levelEndX;
    
    std::string _nextLevel;

    cocos2d::EventListenerKeyboard *eventListener;
    
    cocos2d::PhysicsWorld *_w;
};
} // JTTW

#endif // MainGameSceneScene_h
