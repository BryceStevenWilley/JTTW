#include "EndLevelCutscene.hpp"
#include "FShake.h"
#include "Viewpoint.hpp"

using namespace JTTW;

EndLevelCutscene::EndLevelCutscene(Scene uid, Viewpoint vp) : Cutscene(uid, vp) {

}

void EndLevelCutscene::runScene(bool goToStartBlackout, cocos2d::Scene *nextScene) {
    // TODO: temp, remove.
    goToStartBlackout = true;
    
   // _eventDispatcher->removeEventListener(_listener);
    //_eventDispatcher->removeEventListener(_keySync);
    //_eventDispatcher->addEventListenerWithSceneGraphPriority(_keySync, _scene);

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
        
        // Define parameters for each of the levels.
        auto fadeToBlackDuration = 0.0;
        auto revealDuration = 0.0;
        auto waitDuration = 0.0;
        auto endFadeDuration = 0.0;
        switch (which) {
            case FLIGHT:
                fadeToBlackDuration = 0.5;
                revealDuration = 0.5;
                waitDuration = 3.0;
                endFadeDuration = 2.0;
                break;
                
            case BODHI:
                fadeToBlackDuration = 0.5;
                revealDuration = 1.5;
                waitDuration = 10.0;
                endFadeDuration = 3.0;
                break;
            
            default:
                std::cout << which << " is a new cut scene. ADD IT IN." << std::endl;
                break;
        }
        
        auto fadeToBlack = cocos2d::FadeIn::create(fadeToBlackDuration);
        auto showCutsceneBars = cocos2d::CallFunc::create([this]() {
        });
        auto revealScene = cocos2d::FadeOut::create(revealDuration);
        auto wait = cocos2d::ScaleBy::create(waitDuration, 1.0);
        auto fadeOutOfLevel = cocos2d::FadeIn::create(endFadeDuration);
        
        
        nextScene->retain();
        auto cleanUp = cocos2d::CallFunc::create([this, nextScene]() {
            cocos2d::Director::getInstance()->replaceScene(nextScene);
        });
        
        auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
        cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
        auto middlePoint = origin + visibleSize / 2.0;
        
        // Do all of the actual actor creation and moving.
        cocos2d::CallFunc *createActorsInScene;
        cocos2d::CallFunc *moveActorsInScene;
        switch (which) {
            case FLIGHT: {
                auto hand = cocos2d::Sprite::create("assets/Fist.png");
                hand->setContentSize(cocos2d::Size(ideal2Res(360), ideal2Res(600)));
                auto bg = cocos2d::Sprite::create("assets/bgSunny.png");
                auto blueGround = cocos2d::Sprite::create("assets/blueGround.png");
                blueGround->setContentSize(cocos2d::Size(visibleSize.width, visibleSize.height * (1.0 / 2.0)));
                blueGround->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
                blueGround->setPosition(0, -3000);
                bg->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
                bg->setContentSize(cocos2d::Size(10000, 5000));
                bg->setPosition(0, -3000);
                hand->setPosition(middlePoint + cocos2d::Vec2(0.0, visibleSize.height * (1.0/4.0)));
                hand->retain();
                bg->retain();
                blueGround->retain();
                createActorsInScene = cocos2d::CallFunc::create([this, hand, bg, blueGround]() {
                    // Set the real scene to not be visible.
                    _scene->layer->setVisible(false);
                    _scene->addChild(hand, 1);
                    _scene->addChild(bg, 0);
                    _scene->addChild(blueGround, 1);
                });
        
                moveActorsInScene = cocos2d::CallFunc::create([this, hand, bg, blueGround]() {
                    hand->runAction(cocos2d::Sequence::create(cocos2d::ScaleBy::create(1.9, 1.0),
                                                              FShake::actionWithDuration(0.5, 7.0f),
                                                              nullptr));
                    bg->runAction(cocos2d::Sequence::create(cocos2d::MoveBy::create(2.0, cocos2d::Vec2(0, 3000)),
                                                            nullptr));
                    blueGround->runAction(cocos2d::Sequence::create(cocos2d::MoveBy::create(1.9, cocos2d::Vec2(0, 3000)),
                                                                    FShake::actionWithDuration(0.5, 1.0f),
                                                                    nullptr));
                });
                break;
            }
            
            case BODHI: {
                auto characters = _scene->characters;
                Platform *bodhiTree = _scene->platforms[1];
                std::cout << _scene->platforms.size() << std::endl;
                Viewpoint vp = _scene->vp;
                createActorsInScene = cocos2d::CallFunc::create([this, characters, bodhiTree, vp]() mutable {
                    double startX = 1462.4;
                    for (auto& c: characters) {
                        c->setPosition(cocos2d::Vec2(ideal2Res(startX), ideal2Res(160.908)));
                        startX += 30;
                    }
                    bodhiTree->body->removeFromWorld();
                    vp.followCharacter(bodhiTree, 0.1);
                });
                
                moveActorsInScene = cocos2d::CallFunc::create([this, characters, vp, bodhiTree]() mutable{
                    _scene->layer->runAction(cocos2d::ScaleBy::create(3.0, 1.25));
                    _scene->layer->runAction(cocos2d::MoveBy::create(3.0, cocos2d::Vec2(-ideal2Res(400), 0.0)));
                    cocos2d::CallFunc *moveRight1 = cocos2d::CallFunc::create([characters]() {
                        characters[0]->impulseRight(ideal2Res(140.0));
                    });
                    auto stopRight1 = cocos2d::CallFunc::create([characters]() {
                        characters[0]->impulseRight(-ideal2Res(140.0));
                        characters[0]->setTimeScale(0.3);
                    });
                    cocos2d::CallFunc *moveRight2 = cocos2d::CallFunc::create([characters]() {
                        characters[1]->impulseRight(ideal2Res(140.0));
                    });
                    auto stopRight2 = cocos2d::CallFunc::create([characters]() {
                        characters[1]->impulseRight(-ideal2Res(140.0));
                        characters[1]->setTimeScale(0.3);
                    });
                    cocos2d::CallFunc *moveRight3 = cocos2d::CallFunc::create([characters]() {
                        characters[2]->impulseRight(ideal2Res(140.0));
                    });
                    auto stopRight3 = cocos2d::CallFunc::create([characters]() {
                        characters[2]->impulseRight(-ideal2Res(140.0));
                        characters[2]->setScaleX(std::abs(characters[3]->getScaleX()) * -1);
                        characters[2]->setTimeScale(0.3);
                    });
                    cocos2d::CallFunc *moveRight4 = cocos2d::CallFunc::create([characters]() {
                        characters[3]->impulseRight(ideal2Res(140.0));
                    });
                    auto stopRight4 = cocos2d::CallFunc::create([characters]() {
                        characters[3]->impulseRight(-ideal2Res(140.0));
                        characters[3]->setScaleX(std::abs(characters[3]->getScaleX()) * -1);
                        characters[3]->setTimeScale(0.3);
                    });
                    auto turn1 = cocos2d::CallFunc::create([characters]() {
                        characters[0]->setScaleX(-std::abs(characters[0]->getScaleX()));
                    });
                    auto turn2 = cocos2d::CallFunc::create([characters]() {
                        characters[1]->setScaleX(-std::abs(characters[1]->getScaleX()));
                    });
                    auto turn3 = cocos2d::CallFunc::create([characters]() {
                        characters[2]->setScaleX(std::abs(characters[2]->getScaleX()));
                    });
                    auto turn4 = cocos2d::CallFunc::create([characters]() {
                        characters[3]->setScaleX(std::abs(characters[3]->getScaleX()));
                    });
                    
                    characters[0]->runAction(cocos2d::Sequence::create(moveRight1, pauseFor(1.1), stopRight1, pauseFor(1), turn1, nullptr));
                    characters[1]->runAction(cocos2d::Sequence::create(moveRight2, pauseFor(.8), stopRight2, pauseFor(3), turn2, nullptr));
                    characters[2]->runAction(cocos2d::Sequence::create(moveRight3, pauseFor(3.5), stopRight3, pauseFor(4), turn3, nullptr));
                    characters[3]->runAction(cocos2d::Sequence::create(moveRight4, pauseFor(3.3), stopRight4, pauseFor(4.78), turn4, nullptr));
                });
                break;
            
            }
            
            default:
                std::cout << which << " is a new cutscene. ADD IT IN." << std::endl;
                createActorsInScene = cocos2d::CallFunc::create([]() {});
                moveActorsInScene = cocos2d::CallFunc::create([]() {});
                break;
        }
        auto seq = cocos2d::Sequence::create(prep, fadeToBlack, createActorsInScene, revealScene, moveActorsInScene, wait, fadeOutOfLevel, cleanUp, nullptr);
  
        _curtain->runAction(seq);
    }
}
