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

namespace JTTW {
    
class HelloWorld : public cocos2d::Layer {
public:
    static cocos2d::Scene* createScene();
    virtual bool init() override;
    void menuCloseCallback(cocos2d::Ref* pSender);
    void switchToCharacter(int charIndex);
    
    CREATE_FUNC(HelloWorld);
    
    void update(float) override;
    
    bool onContactBegin(cocos2d::PhysicsContact &contact);
    
private:
    cocos2d::Node *body;
    static Viewpoint vp;
    static std::vector<Character *> characters;
    static std::vector<AiAgent *> agents;
    static AiAgent *player;
    static std::vector<Platform *> platforms;
    static std::vector<MoveablePlatform *> moveables;
    static bool debugOn; // currently, will just turn on collision boxes.
    //static bool pedestalPopped;
    //static bool cloudSunk;
    //static bool cloudSinking;
};
} // JTTW

#endif // HelloWorldScene_h
