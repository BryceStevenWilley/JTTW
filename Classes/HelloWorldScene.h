#ifndef HelloWorldScene_h
#define HelloWorldScene_h

#include "cocos2d.h"
#include "Character.h"
#include "Platform.hpp"
#include "AiAgent.hpp"
#include "Viewpoint.hpp"
#include "MoveablePlatform.hpp"
#include <spine/spine-cocos2dx.h>
#include <deque>
#include "json.hpp"

namespace JTTW {
    
class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init() override;
    void menuCloseCallback(cocos2d::Ref* pSender);
    void switchToCharacter(int charIndex);
    
    CREATE_FUNC(HelloWorld);
    
    void update(float) override;
    
private:
    Viewpoint vp = Viewpoint(cocos2d::Size(1.0, 1.0), 1.0, nullptr);
    std::vector<Character *> characters = std::vector<Character *>();
    std::vector<AiAgent *> agents = std::vector<AiAgent *>();
    AiAgent *player = nullptr;
    std::vector<GameObject *> platforms = std::vector<GameObject *>();
    std::vector<MoveablePlatform *> moveables = std::vector<MoveablePlatform *>();
    bool debugOn = true; // currently, will just turn on collision boxes.

    //static bool pedestalPopped;
    //static bool cloudSunk;
    //static bool cloudSinking;
};
} // JTTW

#endif // HelloWorldScene_h
