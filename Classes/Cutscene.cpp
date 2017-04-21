#include "Cutscene.hpp"


using namespace JTTW;

Cutscene::Cutscene(Cutscene::Scene uid, Viewpoint vp) {
    // Just find the unique cutscene id, and hand-load it. Dumb, but works.
    which = uid;
    switch (uid) {
        case DRAGON: {
            charactersStart["Monkey"] = cocos2d::Vec2(2772.67, 539.289);
            charactersStart["Monk"] = cocos2d::Vec2(2912.4, 539.289);
            charactersEnd["Monkey"] = cocos2d::Vec2(5460.93, 356.097);
            charactersEnd["Monk"] = cocos2d::Vec2(5652.92, 333.034);
            charactersEnd["Sandy"] = cocos2d::Vec2(5241.66, 346.106);
            
            triggeringCharacters.push_back("Monk");
        }
        
        default:
            std::cout << "Nothing happens for other scenes like " << (int) uid << " in cutscene." << std::endl;
    }
}

bool Cutscene::trigger() {
    runScene(true);
    return true;
}

void Cutscene::initFromScene(MainGameScene *scene) {
     for (auto &c : scene->characters) {
         _characters[c->characterName] = c;
     }
     if (which == DRAGON) {
         charactersStart["Sandy"] = _characters["Sandy"]->getPosition();
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
            CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("Music/TrucePt2.mp3");
            _scene->KeypressedCallback(cocos2d::EventKeyboard::KeyCode::KEY_0, nullptr);
            for (auto entry = _characters.begin(); entry != _characters.end(); entry++) {
                entry->second->stop();
            }
            _curtain->setOpacity(0.0);
            _curtain->setVisible(true);
        });
        auto fadeIn = cocos2d::FadeIn::create(2.0);
        auto setupActors = cocos2d::CallFunc::create([this]() {
            for (auto entry = _characters.begin(); entry != _characters.end(); entry++) {
                entry->second->setPosition(charactersStart[entry->first]);
            }
        });

        auto revealScene = cocos2d::FadeOut::create(2.0);
        
        auto moveActorsInScene = cocos2d::CallFunc::create([this]() {
            auto moveDragon = cocos2d::CallFunc::create([this]() {
                _scene->dragon->move(cocos2d::Vec2(100, 0));
            });
            
            _scene->dragon->runAction(moveDragon);
        });
        
        auto wait = cocos2d::ScaleBy::create(5.0, 1.0);
        
        auto fadeToControl = cocos2d::FadeIn::create(2.0);
                
        auto setEndPositions = cocos2d::CallFunc::create([this]() {
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
            _scene->KeypressedCallback(cocos2d::EventKeyboard::KeyCode::KEY_1, nullptr);
            _scene->dragon->removeFromParent();
        });
        auto seq = cocos2d::Sequence::create(prep, fadeIn, setupActors, revealScene, moveActorsInScene, wait, fadeToControl, setEndPositions, fadeOut, cleanUp, nullptr);
  
        _curtain->runAction(seq);
    }
}
