#include "Cutscene.hpp"

using namespace JTTW;

Cutscene::Cutscene(int uid, Viewpoint vp, Zone z) : _triggerZone(z) {
    // Just find the unique cutscene id, and hand-load it. Dumb, but works.
    switch (uid) {
        case DRAGON_CUTSCENE: {
            //charactersStart
            charactersEnd["Monkey"] = cocos2d::Vec2(5460.93, 356.097);
            charactersEnd["Monk"] = cocos2d::Vec2(5652.92, 333.034);
            charactersEnd["Sandy"] = cocos2d::Vec2(5241.66, 346.106);
            
            triggeringCharacters.push_back("Monk");
        }
    }
}

bool Cutscene::trigger() {
    bool trigger = true;
    
    for (auto &tc : triggeringCharacters) {
        trigger = trigger && _triggerZone.containsPoint(_characters[tc]->getPosition());
    }
    
    if (trigger) {
        runScene(true);
        return true;
    }
    return false;
}

void Cutscene::initFromScene(MainGameScene *scene) {
     for (auto &c : scene->characters) {
         _characters[c->characterName] = c;
     }
     _scene = scene;
     _curtain = scene->curtain;
     _listener = scene->eventListener;
     _haveReleased = scene->haveReleased;
     _eventDispatcher = scene->_eventDispatcher;
     _keySync = scene->pauseListener;
}

void Cutscene::runScene(bool goToStartBlackout) {
    // TODO: temp, remove.
    goToStartBlackout = true;
    
    _eventDispatcher->removeEventListener(_listener);
    _eventDispatcher->removeEventListener(_keySync);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_keySync, _scene);

    if (goToStartBlackout) {
        std::cout << "Running cutscene" << std::endl;
        auto prep = cocos2d::CallFunc::create([this]() {
            _scene->KeypressedCallback(cocos2d::EventKeyboard::KeyCode::KEY_0, nullptr);
            for (auto entry = _characters.begin(); entry != _characters.end(); entry++) {
                entry->second->stop();
            }
            _curtain->setOpacity(0.0);
            _curtain->setVisible(true);
        });
        auto fadeIn = cocos2d::FadeIn::create(2.0);
        
        auto characterMove = cocos2d::CallFunc::create([this]() {
            for (auto entry = _characters.begin(); entry != _characters.end(); entry++) {
                entry->second->setNewRespawn(charactersEnd[entry->first]);
                entry->second->setPosition(charactersEnd[entry->first]);
                entry->second->die(Character::CauseOfDeath::CUTSCENE);
            }
            for (auto agent : _scene->agents) {
                agent->adjustOffset(_scene->player->_controlledCharacter);
            }
        });
        
        auto fadeOut = cocos2d::FadeOut::create(2.0);
        auto cleanUp = cocos2d::CallFunc::create([this]() {
            _curtain->setOpacity(170);
            _curtain->setVisible(false);
            
            _eventDispatcher->removeEventListener(_keySync);
            _eventDispatcher->removeEventListener(_listener);
            _eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, _scene);
 
            _scene->haveReleased.clear();
            _scene->KeypressedCallback(cocos2d::EventKeyboard::KeyCode::KEY_2, nullptr);
        });
        auto seq = cocos2d::Sequence::create(prep, fadeIn, characterMove, fadeOut, cleanUp, nullptr);
  
        _curtain->runAction(seq);
    }
}
