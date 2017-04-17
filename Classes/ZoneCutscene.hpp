#ifndef ZoneCutscene_hpp
#define ZoneCutscene_hpp

#include <stdio.h>
#include "Cutscene.hpp"

namespace JTTW {
class ZoneCutscene : public Cutscene {
public:
    ZoneCutscene(Scene uid, Viewpoint vp, Zone z);

    bool trigger() override;

private:

    Zone _triggerZone;

};
}

#endif /* ZoneCutscene_hpp */
