#ifndef LevelSelectScene_hpp
#define LevelSelectScene_hpp

#include "cocos2d.h"
#include <iostream>

namespace JTTW {

class LevelSelect : public cocos2d::Layer {
public:
    static cocos2d::Scene* createScene();
    virtual bool init() override;
    
    CREATE_FUNC(LevelSelect);
    
    void menuCallback(std::string newLevel);
    
private:
    cocos2d::EventListenerKeyboard *keyListener;
    cocos2d::Label *levelName;
    cocos2d::Label *devMode;
    std::map<int, std::string> tagToFileName;
    std::vector<std::string> allLevels;
    std::vector<std::string>::iterator currentLevel;
    cocos2d::Label *titleLabel;
};

}

#endif /* LevelSelectScene_hpp */
