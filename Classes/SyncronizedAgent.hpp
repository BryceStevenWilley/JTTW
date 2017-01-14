#ifndef SyncronizedAgent_hpp
#define SyncronizedAgent_hpp

#include <stdio.h>
#include "AiAgent.hpp"

namespace JTTW {
    /**
     * This agent will attempt to do exactly as the controlling player does, i.e.
     * moving and jumping at the exact same time.
     */
    class SyncronizedAgent: public AiAgent {
    public:
        SyncronizedAgent(Character *controlledCharacter): AiAgent(controlledCharacter) {}
        void plan(Character *player, std::vector<Character *> otherCharacters);
        void executePlan(float delta);
        void setMap();
    private:
        std::queue<ActionAndTrigger> plannedActions;
    };
};

#endif /* SyncronizedAgent_hpp */
