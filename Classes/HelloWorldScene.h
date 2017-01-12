#ifndef HelloWorldScene_h
#define HelloWorldScene_h

#include "cocos2d.h"
#include "Character.h"

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
    static std::vector<Character *> characters;
    static std::vector<Character *>::iterator player;
    
};
}; // JTTW

#endif // HelloWorldScene_h
