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
    static std::set<cocos2d::EventKeyboard::KeyCode> keyPresses;
    Character *transformer;
    
};
}; // JTTW

#endif // HelloWorldScene_h
