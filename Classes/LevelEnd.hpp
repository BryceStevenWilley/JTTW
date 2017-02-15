//
//  LevelEnd.hpp
//  JTTW
//
//  Created by Bryce Willey on 2/14/17.
//
//

#ifndef LevelEnd_hpp
#define LevelEnd_hpp

#include <iostream>

namespace JTTW {

class LevelEnd : public cocos2d::Layer {
public:
    static cocos2d::Scene* createScene(std::string &nextLevelToLoad);
    virtual bool init(std::string &nextLevelToLoad);
    
    //CREATE_FUNC(LevelEnd);
    static LevelEnd* create(std::string &nextLevelToLoad){
        LevelEnd *pRet = new LevelEnd();
        if (pRet && pRet->init(nextLevelToLoad)) {
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
