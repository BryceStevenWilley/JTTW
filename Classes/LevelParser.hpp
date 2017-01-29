#ifndef LevelParser_hpp
#define LevelParser_hpp

#include <stdio.h>
#include "json.hpp"
#include "Platform.hpp"
#include "Viewpoint.hpp"

namespace JTTW {
    void parseLevelFromJson(std::string fileName, cocos2d::Layer *layer, std::vector<Platform> &platforms, Viewpoint vp, bool debugOn);
}

#endif /* LevelParser_hpp */
