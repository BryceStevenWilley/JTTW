#ifndef LevelParser_hpp
#define LevelParser_hpp

#include <stdio.h>
#include "cocos2d.h"
#include "json.hpp"
#include "Platform.hpp"
#include "Viewpoint.hpp"
#include "MoveablePlatform.hpp"

namespace JTTW {
    nlohmann::json parseLevelFromJson(std::string fileName, cocos2d::Layer *level, cocos2d::Layer *platLayer, std::vector<Platform *> &platforms, std::vector<MoveablePlatform *> &movables, Viewpoint vp, bool debugOn);
}

#endif /* LevelParser_hpp */
