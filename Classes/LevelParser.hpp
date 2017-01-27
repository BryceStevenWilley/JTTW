//
//  LevelParser.hpp
//  JTTW
//
//  Created by Bryce Willey on 1/27/17.
//
//

#ifndef LevelParser_hpp
#define LevelParser_hpp

#include <stdio.h>
#include "json.hpp"
#include "Platform.hpp"
#include "Viewpoint.hpp"

namespace JTTW {
    void parseLevelFromJson(std::string fileName, cocos2d::Layer *layer, std::vector<Platform> &platforms, Viewpoint vp);
}

#endif /* LevelParser_hpp */
