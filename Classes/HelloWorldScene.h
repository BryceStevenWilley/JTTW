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
    virtual bool init(std::string levelToLoad); //override;
    void menuCloseCallback(cocos2d::Ref* pSender);
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
    
private:
    cocos2d::Node *body;
    Viewpoint vp = Viewpoint(cocos2d::Size(1.0, 1.0), 1.0, nullptr);
    std::vector<Character *> characters = std::vector<Character *>();
    std::vector<AiAgent *> agents = std::vector<AiAgent *>();
    AiAgent *player = nullptr;
    std::vector<Platform *> platforms = std::vector<Platform *>();
    std::vector<MoveablePlatform *> moveables = std::vector<MoveablePlatform *>();
    bool debugOn = true; // currently, will just turn on collision boxes.

    //static bool pedestalPopped;
    //static bool cloudSunk;
    //static bool cloudSinking;
};
} // JTTW

#endif // HelloWorldScene_h
