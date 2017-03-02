#ifndef MainMenuScene_hpp
#define MainMenuScene_hpp

#include "cocos2d.h"
#include <iostream>

namespace JTTW {

class MainMenu : public cocos2d::Layer {
public:
    static cocos2d::Scene*createScene();
    virtual bool init() override;
    
    CREATE_FUNC(MainMenu);
    
    void openStartScene();
    
    void openSettings();
    
    void exitGame();
    
private:
    void addCharacterAni(std::string name, cocos2d::Vec2);

    cocos2d::EventListenerKeyboard *eventListener;
    
    std::vector<cocos2d::MenuItemSprite *> options;
    std::vector<cocos2d::MenuItemSprite *>::iterator currentOption;
};
} // JTTW

#endif /* MainMenuScene_hpp */
