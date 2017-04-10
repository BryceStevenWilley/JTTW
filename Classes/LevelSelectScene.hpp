#ifndef LevelSelectScene_hpp
#define LevelSelectScene_hpp

#include "cocos2d.h"
#include <iostream>

namespace JTTW {


struct LevelInfoStruct {
    std::string title;
    std::string path;
    std::string fileName;
    cocos2d::Sprite * thumbnail;
    int order;
    
    bool operator < (const LevelInfoStruct &other) const {
        return this->order < other.order;
    }
};


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
    std::vector<LevelInfoStruct> allLevels;
    std::map<int, std::string> tagToFileName;
    unsigned int currentLevel;
    cocos2d::Label *titleLabel;
    
    cocos2d::Size visibleSize;
    cocos2d::Vec2 origin;
};
}
#endif /* LevelSelectScene_hpp */
