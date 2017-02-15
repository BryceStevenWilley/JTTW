#ifndef HelloWorldScene_h
#define HelloWorldScene_h

#include "cocos2d.h"
#include "Character.hpp"
#include "Platform.hpp"
#include "AiAgent.hpp"
#include "Viewpoint.hpp"
#include "MoveablePlatform.hpp"
#include <spine/spine-cocos2dx.h>
#include <deque>
#include "json.hpp"

namespace JTTW {
    
class HelloWorld : public cocos2d::Layer {
public:
    static cocos2d::Scene* createScene(std::string levelToLoad);
    virtual bool init(std::string levelToLoad);
    void menuCloseCallback(cocos2d::Ref* pSender);
    void nextLevelCallback();
    void switchToCharacter(int charIndex);
    
    //CREATE_FUNC(HelloWorld);
    static HelloWorld* create(std::string levelToLoad){
        HelloWorld *pRet = new HelloWorld();
        if (pRet && pRet->init(levelToLoad)) {
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
    std::vector<cocos2d::Sprite *> dynamics = std::vector<cocos2d::Sprite *>();
    AiAgent *player = nullptr;
    std::vector<Platform *> platforms = std::vector<Platform *>();
    std::vector<MoveablePlatform *> moveables = std::vector<MoveablePlatform *>();
    bool debugOn = true; // currently, will just turn on collision boxes.

    bool nextLevelStarting = false;
    bool firstClick = false;

    double levelEndX;
    
    std::string _nextLevel;

    cocos2d::EventListenerKeyboard *eventListener;
    //static bool pedestalPopped;
    //static bool cloudSunk;
    //static bool cloudSinking;
};
} // JTTW

#endif // HelloWorldScene_h
