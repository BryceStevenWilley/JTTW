#ifndef LevelEnd_hpp
#define LevelEnd_hpp

#include "cocos2d.h"
#include <iostream>

namespace JTTW {

class LevelEnd : public cocos2d::Layer {
public:
    static cocos2d::Scene* createScene(std::string &nextLevelToLoad, std::string &endQuote);
    virtual bool init(std::string &nextLevelToLoad, std::string &endQuote);
    
    //CREATE_FUNC(LevelEnd);
    static LevelEnd* create(std::string &nextLevelToLoad, std::string &endQuote){
        LevelEnd *pRet = new LevelEnd();
        if (pRet && pRet->init(nextLevelToLoad, endQuote)) {
            pRet->autorelease();
            return pRet;
        }
        else {
            delete pRet;
            pRet = NULL;
            return NULL;
        }
    }
    
    void menuCallback();
    
private:
    std::string _nextLevelToLoad;
    cocos2d::EventListenerKeyboard *keyListener;
    cocos2d::EventListenerMouse *mouseListener;
};

}

#endif /* LevelEnd_hpp */
