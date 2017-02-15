//
//  LevelSelectScene.hpp
//  JTTW
//
//  Created by Bryce Willey on 2/4/17.
//
//

#ifndef LevelSelectScene_hpp
#define LevelSelectScene_hpp

#include <iostream>

namespace JTTW {

class LevelSelect : public cocos2d::Layer {
public:
    static cocos2d::Scene* createScene();
    virtual bool init() override;
    
    CREATE_FUNC(LevelSelect);
    
    void menuCallback(cocos2d::Ref *fromItem);
    
private:
    std::map<int, std::string> tagToFileName;
};

}

#endif /* LevelSelectScene_hpp */
