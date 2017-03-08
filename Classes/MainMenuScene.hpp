#ifndef MainMenuScene_hpp
#define MainMenuScene_hpp

#include "cocos2d.h"
#include <spine/spine-cocos2dx.h>
#include <iostream>

namespace JTTW {

cocos2d::Label * createSunriseLabel(std::string content, int size, float scaleFactor);

class MainMenu : public cocos2d::Layer {
public:
    static cocos2d::Scene*createScene();
    virtual bool init() override;
    
    CREATE_FUNC(MainMenu);
    
    void openStartScene();
    
    void openFirstLevel();
    
    void exitGame();
    
private:
    spine::SkeletonAnimation *addCharacterAni(std::string name, cocos2d::Vec2 startPosition, cocos2d::Vec2 endPosition);

    cocos2d::EventListenerKeyboard *eventListener;
    
    std::vector<cocos2d::MenuItem *> options;
    std::vector<cocos2d::MenuItem *>::iterator currentOption;
    
    const cocos2d::Color3B SELECTED = cocos2d::Color3B(206.0, 208.0, 103.0);
    const cocos2d::Color3B UNSELECTED = cocos2d::Color3B(255.0, 255.0, 255.0);
};
} // JTTW

#endif /* MainMenuScene_hpp */
