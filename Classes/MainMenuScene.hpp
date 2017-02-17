#ifndef MainMenuScene_hpp
#define MainMenuScene_hpp

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
    cocos2d::EventListenerKeyboard *eventListener;
};
} // JTTW

#endif /* MainMenuScene_hpp */
