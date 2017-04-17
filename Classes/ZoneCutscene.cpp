#include "ZoneCutscene.hpp"

using namespace JTTW;

ZoneCutscene::ZoneCutscene(Scene uid, Viewpoint vp, Zone z) : Cutscene(uid, vp), _triggerZone(z) {

}


bool ZoneCutscene::trigger() {
    bool trigger = true;
    
    for (auto &tc : triggeringCharacters) {
        trigger = trigger && _triggerZone.containsPoint(_characters[tc]->getPosition());
    }
    
    if (trigger) {
        return Cutscene::trigger();
    }
    return false;
}
