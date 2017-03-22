#ifndef MainGameSceneScene_h
#define MainGameSceneScene_h

#include "cocos2d.h"
#include "Character.hpp"
#include "Platform.hpp"
#include "AiAgent.hpp"
#include "Viewpoint.hpp"
#include "MoveablePlatform.hpp"
#include "Boulder.hpp"
#include <spine/spine-cocos2dx.h>
#include <map>
#include "Vine.hpp"
#include "Trap.hpp"
#include "json.hpp"
#include "Peg.hpp"
#include "SimpleAudioEngine.h"

namespace JTTW {

struct Zone {
private:
    cocos2d::Vec2 bottomLeft;
    cocos2d::Vec2 topRight;
    
public:
    Zone(cocos2d::Vec2 a, cocos2d::Vec2 b): bottomLeft(a), topRight(b) {}
    
    bool containsPoint(cocos2d::Vec2 p) {
        return (p.x >= bottomLeft.x && p.x <= topRight.x && p.y >= bottomLeft.y && p.y <= topRight.y);
    }
};

    
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
    bool characterCollision(cocos2d::PhysicsContact& contact, bool begin, Character *c, cocos2d::PhysicsBody *body, cocos2d::Node *node, cocos2d::Vec2 normal);

    Viewpoint vp = Viewpoint(cocos2d::Size(1.0, 1.0), 1.0);
    std::vector<Character *> characters = std::vector<Character *>();
    std::vector<AiAgent *> agents = std::vector<AiAgent *>();
    AiAgent *player = nullptr;
    
    std::vector<Trap *> trapsToTrigger = std::vector<Trap *>();
    std::vector<Platform *> platforms = std::vector<Platform *>();
    std::vector<MoveablePlatform *> moveables = std::vector<MoveablePlatform *>();
    std::vector<Platform *> disappearing = std::vector<Platform *>();
    std::vector<Vine *> vines = std::vector<Vine *>();
    std::vector<cocos2d::Vec2> respawnPoints = std::vector<cocos2d::Vec2>();
    std::vector<Peg *> pegs = std::vector<Peg *>();
    std::vector<Zone> attackZones = std::vector<Zone>();
 
    std::map<int, Boulder *> boulders; 
    std::map<int, std::vector<int>> joints = std::map<int, std::vector<int>>();
    std::map<Character *, bool> attacking = std::map<Character *, bool>();
    std::map<Character *, double> attackCountdown = std::map<Character *, double>();

    CocosDenshion::SimpleAudioEngine *audio;

    bool debugOn = true; // currently, will just turn on collision boxes.

    bool nextLevelStarting = false;
    bool afterFirstClick = false;

    double levelEndX;
    
    std::string _nextLevel;

    cocos2d::EventListenerKeyboard *eventListener;
    
    cocos2d::PhysicsWorld *_w;
    
    cocos2d::Layer *layer;
    cocos2d::Layer *uiLayer;
};
} // JTTW

#endif // MainGameSceneScene_h
