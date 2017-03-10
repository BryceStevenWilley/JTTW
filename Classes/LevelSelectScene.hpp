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
    void findLevelFiles(bool includeDev);

    cocos2d::EventListenerKeyboard *keyListener;
    cocos2d::Label *levelName;
    cocos2d::Label *devMode;
    std::map<int, std::string> tagToFileName;
    std::vector<std::string> allLevelPaths;
    std::vector<std::string> allLevelNames;
    std::vector<cocos2d::Sprite *> allLevelThumbnails;
    unsigned int currentLevel;
    cocos2d::Label *titleLabel;
    
    cocos2d::Size visibleSize;
    cocos2d::Vec2 origin;
};
}
#endif /* LevelSelectScene_hpp */
