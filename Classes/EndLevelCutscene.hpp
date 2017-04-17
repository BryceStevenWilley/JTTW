#ifndef EndLevelCutscene_hpp
#define EndLevelCutscene_hpp

#include <iostream>
#include "Cutscene.hpp"

namespace JTTW {

class EndLevelCutscene: public Cutscene {
public:
    EndLevelCutscene(Scene uid, Viewpoint vp);

    void runScene(bool gotoStartBlackout, cocos2d::Scene *nextScene);
};

}


#endif /* EndLevelCutscene_hpp */
