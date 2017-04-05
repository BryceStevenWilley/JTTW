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
#include "ProjectileFactory.hpp"
#include "Zone.hpp"

namespace JTTW {

class Cutscene;
    
class MainGameScene : public cocos2d::Layer {
friend class Cutscene;
public:
    static cocos2d::Scene* createScene(std::string levelToLoad);
    virtual bool init(std::string levelToLoad, cocos2d::PhysicsWorld *w);
    void menuCloseCallback();
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
    bool onContactPostSolve(cocos2d::PhysicsContact &contact);
    bool onContactHandler(cocos2d::PhysicsContact &contact, bool begin);
 
 
    //////////////// Pause scene stuff ////////////////////
    void pauseScene();
    void resumeScene();
 
private:
    cocos2d::Layer *parseLevelFromJsonV2(nlohmann::json fileName, bool debugOn);
    bool characterCollision(cocos2d::PhysicsContact& contact, bool begin, Character *c, cocos2d::PhysicsBody *body, cocos2d::Node *node, cocos2d::Vec2 normal);
    void KeypressedCallback(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
    void PausedKeyCallback(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
    void KeyreleasedCallback(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
    void PausedKeyReleasedCallback(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);

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
    std::vector<Cutscene *> cutscenes = std::vector<Cutscene *>();
 
    std::map<int, Boulder *> boulders; 
    std::map<int, std::vector<int>> joints = std::map<int, std::vector<int>>();
    std::map<Character *, std::vector<ProjectileFactory *>> attacking = std::map<Character *, std::vector<ProjectileFactory *>>();
    std::map<Character *, double> attackCountdown = std::map<Character *, double>();
    std::map<cocos2d::Sprite *, double> deleteTimer = std::map<cocos2d::Sprite *, double>();

    CocosDenshion::SimpleAudioEngine *audio;

    bool debugOn = true; // currently, will just turn on collision boxes.

    bool nextLevelStarting = false;
    std::map<cocos2d::EventKeyboard::KeyCode, bool> haveReleased = std::map<cocos2d::EventKeyboard::KeyCode, bool>();
    std::map<cocos2d::EventKeyboard::KeyCode, bool> stillPressed = std::map<cocos2d::EventKeyboard::KeyCode, bool>();

    cocos2d::Vec2 levelEnd;
    int levelEndDir = 4;
    
    std::string _nextLevel;

    cocos2d::EventListenerKeyboard *eventListener;
    
    cocos2d::PhysicsWorld *_w;
    
    cocos2d::Layer *layer;
    cocos2d::Layer *uiLayer;
    cocos2d::Sprite *background;
    cocos2d::Sprite *blackBarUp;
    cocos2d::Sprite *blackBarDown;
    
    cocos2d::Vec2 upStart;
    cocos2d::Vec2 upVisible;
    cocos2d::Vec2 downStart;
    cocos2d::Vec2 downVisible;
    
    ////////////// Pause Scene Stuff /////////////
    cocos2d::EventListenerKeyboard *pauseListener;
    
    std::vector<cocos2d::MenuItem *> options;
    std::vector<cocos2d::MenuItem *>::iterator currentOption;
    cocos2d::Menu *m;
    
    cocos2d::Sprite *curtain;
    
    const cocos2d::Color3B SELECTED = cocos2d::Color3B(206.0, 208.0, 103.0);
    const cocos2d::Color3B UNSELECTED = cocos2d::Color3B(255.0, 255.0, 255.0);
};
} // JTTW

#endif // MainGameSceneScene_h
