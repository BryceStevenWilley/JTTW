#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "MainMenuScene.hpp"
#include <iostream>
#include "Character.hpp"
#include "LevelEnd.hpp"
#include <stdio.h>
#include "cocos2d.h"
#include "json.hpp"
#include <iostream>
#include <fstream>

using namespace cocos2d;
using namespace JTTW;

//bool HelloWorld::pedestalPopped;
//bool HelloWorld::cloudSunk = false;
//bool HelloWorld::cloudSinking = false;

cocos2d::Scene* HelloWorld::createScene(std::string levelToLoad) {
    // 'scene' and layer are autorelease objects.
    auto scene = cocos2d::Scene::createWithPhysics();
    //scene->autorelease();
    scene->getPhysicsWorld()->setGravity(cocos2d::Vec2(0, -298));
    scene->getPhysicsWorld()->setDebugDrawMask(cocos2d::PhysicsWorld::DEBUGDRAW_ALL);
    auto layer = HelloWorld::create(levelToLoad);
    scene->addChild(layer);

    return scene;
}

bool HelloWorld::onContactHandler(cocos2d::PhysicsContact& contact, bool begin) {
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    cocos2d::Vec2 normal = contact.getContactData()->normal;
    if (nodeA->getTag() == CHARACTER_TAG) {
        Character *c = (Character *)nodeA;
        // If A is the character, then for landing on a flat platform, the normal is 0, -1.
        if (normal.dot(cocos2d::Vec2(0, -1)) > std::cos(M_PI/4.0)) {
            // Character landed on a platform, probably.
            if (begin) {
                c->landedCallback();
            } else { // onContactEnd
                c->leftCallback();
            }
        }
        if (!begin) {
            c->rebalanceImpulse();
        }
    }
    
    if (nodeB->getTag() == CHARACTER_TAG) {
        Character *c = (Character *)nodeB;
        // If B is the character, then for landing on a flat platform, the normal is 0, 1.
        if (normal.dot(cocos2d::Vec2(0, 1)) > std::cos(M_PI /4.0)) {
            if (begin) {
                c->landedCallback();
            } else { // onContactEnd
                c->leftCallback();
            }
        }
        if (!begin) {
            c->rebalanceImpulse();
        }
    }
    return true;
}

bool HelloWorld::onContactBegin(cocos2d::PhysicsContact& contact) {
    return onContactHandler(contact, true);
}

bool HelloWorld::onContactEnd(cocos2d::PhysicsContact& contact) {
    return onContactHandler(contact, false);
}


cocos2d::Layer *HelloWorld::parseLevelFromJson(std::string fileName, bool debugOn) {
    
    cocos2d::Layer *platLayer = cocos2d::Layer::create();
    
    const int platformZ = 4;
    
    std::ifstream inFile(fileName);
    nlohmann::json lvl;
    
    std::cout.flush();
    inFile >> lvl;
    
    // draw and add background
    nlohmann::json backgroundAtts = lvl["background"];
    std::string bgPath = backgroundAtts["imageName"];
    bgPath = "backgrounds/" + bgPath;
    auto background = cocos2d::Sprite::create(bgPath);
    background->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    background->setScale(1.4);
    background->setPosition(0,-300.0);
    this->addChild(background, -8);
    
    nlohmann::json platformAtts = lvl["platforms"];
    for (auto& pAtt: platformAtts) {
        std::string fullImagePath = pAtt["imageName"];
        fullImagePath = "platforms/" + fullImagePath;
        double centerX = vp.metersToPixels((double)pAtt["centerX"]);
        double centerY = vp.metersToPixels((double)pAtt["centerY"]);
        double imageSizeWidth = vp.metersToPixels((double)pAtt["imageSizeWidth"]);
        double imageSizeHeight = vp.metersToPixels((double)pAtt["imageSizeHeight"]);
        
        // Get the collision Width and height.
        double collisionWidth = vp.metersToPixels((double)pAtt["collisionWidth"]);
        double collisionHeight = vp.metersToPixels((double)pAtt["collisionHeight"]);
        
        if (pAtt["moveable"]) {
            cocos2d::Vec2 centerA(centerX, centerY);
            double centerBX = vp.metersToPixels((double)pAtt["center2X"]);
            double centerBY = vp.metersToPixels((double)pAtt["center2Y"]);
            cocos2d::Vec2 centerB(centerBX, centerBY);
            double maximumVelocity = vp.metersToPixels((double)pAtt["maximumVelocity"]);
            MoveablePlatform *p = new MoveablePlatform(fullImagePath, centerA, centerB, cocos2d::Size(imageSizeWidth, imageSizeHeight), cocos2d::Vec2(collisionWidth, collisionHeight), maximumVelocity);

            platLayer->addChild(p->image, platformZ);
            platforms.push_back(p);
            moveables.push_back(p);
        } else {
            Platform *p = new Platform(fullImagePath, cocos2d::Vec2(centerX, centerY), cocos2d::Size(imageSizeWidth, imageSizeHeight), cocos2d::Vec2(collisionWidth, collisionHeight));
        
            platLayer->addChild(p->getImage(), platformZ);
            platforms.push_back(p);
        }
    }
    
    nlohmann::json characterStart = lvl["characters"];
    
    double monkeyStartX = vp.metersToPixels((double)characterStart["Monkey"]["startingXPos"]);
    double monkeyStartY = vp.metersToPixels((double)characterStart["Monkey"]["startingYPos"]);
        
    double monkStartX = vp.metersToPixels((double)characterStart["Monk"]["startingXPos"]);
    double monkStartY = vp.metersToPixels((double)characterStart["Monk"]["startingYPos"]);
    
    double piggyStartX = vp.metersToPixels((double)characterStart["Piggy"]["startingXPos"]);
    double piggyStartY = vp.metersToPixels((double)characterStart["Piggy"]["startingYPos"]);
    
    double sandyStartX = vp.metersToPixels((double)characterStart["Sandy"]["startingXPos"]);
    double sandyStartY = vp.metersToPixels((double)characterStart["Sandy"]["startingYPos"]);
    
    int characterHeight = vp.metersToPixels(1.7);
    int characterWidth = vp.metersToPixels(1.7);

    Character *monkey = new Character("Monkey", PhysicsMaterial(1.0, 0.0, 1.0), cocos2d::Vec2(monkeyStartX, monkeyStartY), cocos2d::Size(characterWidth, characterHeight));
    Character *monk = new Character("Monk", PhysicsMaterial(1.0, 0.0, 1.0), cocos2d::Vec2(monkStartX, monkStartY), cocos2d::Size(characterWidth, characterHeight));
    Character *piggy = new Character("Piggy", PhysicsMaterial(1.0, 0.0, 1.0), cocos2d::Vec2(piggyStartX, piggyStartY), cocos2d::Size(characterWidth, characterHeight));
    Character *sandy = new Character("sandy", PhysicsMaterial(1.0, 0.0, 1.0), cocos2d::Vec2(sandyStartX, sandyStartY), cocos2d::Size(characterWidth, characterHeight));

    characters.push_back(monkey);
    characters.push_back(monk);
    characters.push_back(piggy);
    characters.push_back(sandy);
    
    charactersAtEnd.push_back(false);
    charactersAtEnd.push_back(false);
    charactersAtEnd.push_back(false);
    charactersAtEnd.push_back(false);
    
    levelEndX = lvl["levelEndX"];
    
    _nextLevel = lvl["nextLevelName"];
    
    return platLayer;
}

bool HelloWorld::init(std::string levelToLoad) {
    if ( !Layer::init() ) {
        return false;
    }
    
    // aka window dimensions
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add a "close" icon to exit the program. it's an autorelease object
    //auto closeItem = cocos2d::MenuItemImage::create("CloseNormal.png",
    //                                       "CloseSelected.png",
    //                                       CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    //closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
    //                            origin.y + closeItem->getContentSize().height/2));

    // create menu with the "X" image, it's an autorelease object
    //auto menu = cocos2d::Menu::create(closeItem, NULL);
    //menu->setPosition(Vec2::ZERO);
    
    // add menu with Z-order 1.
    //this->addChild(menu, 1);

   
    // Creates the camera, or viewpoint for this scene.
    // 1.7/130.0 means that 1.7 meters in the game world (average human male height) is represented by 180 pixels on screen.
    vp = Viewpoint(visibleSize, 1.7/130.0);

    cocos2d::Layer *layer = parseLevelFromJson(levelToLoad, debugOn);
 
    vp.setLayer(layer);
 
    for (int i = 0; i < characters.size(); i++) {
        Character *body = characters[i];
        layer->addChild(body, i);
        AiAgent *agent = new AiAgent(body);
        agent->setPlayerPosOffset(body->getPosition() - characters[0]->getPosition());
        agents.push_back(agent);
    }
    
    player = agents[0];
    player->_controlledCharacter->currentCrown->setVisible(true);

    this->addChild(layer, 1);
    
    eventListener = cocos2d::EventListenerKeyboard::create();
    
    eventListener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) mutable {
        switch(keyCode) {
            case EventKeyboard::KeyCode::KEY_Z:
                switchToCharacter(0);
                break;
            case EventKeyboard::KeyCode::KEY_X:
                switchToCharacter(1);
                break;
            case EventKeyboard::KeyCode::KEY_C:
                switchToCharacter(2);
                break;
            case EventKeyboard::KeyCode::KEY_V:
                switchToCharacter(3);
                break;
            case EventKeyboard::KeyCode::KEY_ESCAPE:
                this->menuCloseCallback(nullptr); // TODO: should I be using this nullptr?
                break;
            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            case EventKeyboard::KeyCode::KEY_SPACE:
            case EventKeyboard::KeyCode::KEY_A:
            case EventKeyboard::KeyCode::KEY_O:
                player->plan(characters, keyCode, true);
                for (auto xAgent = agents.begin(); xAgent != agents.end(); xAgent++) {
                    if ((*xAgent) != player) {
                        (*xAgent)->changeBehavior(player->_controlledCharacter, keyCode);
                    }
                }
                break;
                
            default:
                // do nothing.
                break;
        }
    };
    
    eventListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) mutable {
            player->plan(characters, keyCode, false);
    };

    this->_eventDispatcher->addEventListenerWithFixedPriority(eventListener, 1);
    
    auto contactListener = cocos2d::EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin, this);
    contactListener->onContactSeparate = CC_CALLBACK_1(HelloWorld::onContactEnd, this);
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    this->scheduleUpdate();
    return true;
}

void HelloWorld::switchToCharacter(int charIndex) {
    auto nextPlayer = agents[charIndex];
    if (nextPlayer == player) {
        return; // don't do shit!
    }
    nextPlayer->cedeToPlayer(player);
    player = nextPlayer;
    vp.panToCharacter(player->_controlledCharacter);
    
    // Set all of the offsets correctly.
    for (auto xAgent = agents.begin(); xAgent != agents.end(); xAgent++) {
        if ((*xAgent) != player) {
            (*xAgent)->adjustOffset(player->_controlledCharacter);
        }
    }
}

void HelloWorld::menuCloseCallback(Ref* pSender) {
    auto startScene = MainMenu::createScene();
    this->_eventDispatcher->removeEventListener(eventListener);
    cocos2d::Director::getInstance()->replaceScene(startScene);
}

void HelloWorld::nextLevelCallback() {
    if (!nextLevelStarting) {
        this->_eventDispatcher->removeEventListener(eventListener);
        std::cout << "Starting next level, " << _nextLevel << std::endl;
        auto end = LevelEnd::createScene(_nextLevel);
        auto fade = cocos2d::TransitionFade::create(3.0, end);
        cocos2d::Director::getInstance()->replaceScene(fade);
        nextLevelStarting = true;
    }
}

void HelloWorld::update(float delta) {
    for (auto xAgent = agents.begin(); xAgent != agents.end(); xAgent++) {
        if ((*xAgent) != player) {
            (*xAgent)->plan(player->_controlledCharacter, characters);
            (*xAgent)->executeControl(delta);
        }
    }
    
    bool done = true;
    for (int i = 0; i < characters.size(); i++) {
        characters[i]->updateAnimation();
        if (characters[i]->getPosition().y < -100) { // TODO: un-hardcode this.
            characters[i]->restartFromStart();
        }  else if (vp.pixelsToMeters(characters[i]->getPosition().x) < levelEndX) {
            done = false;
        }
    }
    if (done) {
        for (int i = 0; i < characters.size(); i++) {
            characters[i]->stop();
        }
        nextLevelCallback();
    }
    
    for (int i = 0; i < moveables.size(); i++) {
        moveables[i]->move(delta, false);
    }
    vp.followCharacter(player->_controlledCharacter, delta);
}

