#ifndef HelloWorldScene_h
#define HelloWorldScene_h

#include "cocos2d.h"
#include "Character.h"
#include "AiAgent.hpp"
#include "Viewpoint.hpp"
#include <spine/spine-cocos2dx.h>
#include <deque>

namespace JTTW {
    
class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init() override;
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    CREATE_FUNC(HelloWorld);
    
    void update(float) override;
    
private:
    static Viewpoint vp;
    static std::vector<Character *> characters;
    static std::deque<AiAgent *> agents;
    static AiAgent *player;
    static std::vector<BadPlatform> platforms;
};
} // JTTW

#endif // HelloWorldScene_h
