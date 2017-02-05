//
//  MainMenuScene.hpp
//  JTTW
//
//  Created by Bryce Willey on 2/4/17.
//
//

#ifndef MainMenuScene_hpp
#define MainMenuScene_hpp

#include <stdio.h>

namespace JTTW {

class MainMenu : public cocos2d::Layer {
public:
    static cocos2d::Scene*createScene();
    virtual bool init() override;
    
    CREATE_FUNC(MainMenu);
    
    void openStartScene();
    
    void openSettings();
    
    void exitGame();
    
};
} // JTTW

#endif /* MainMenuScene_hpp */
