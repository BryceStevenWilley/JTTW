#include "MainGameScene.h"
#include "SimpleAudioEngine.h"
#include "MainMenuScene.hpp"
#include "Collisions.hpp"
#include "Monkey.hpp"
#include "LevelEnd.hpp"
#include "cocos2d.h"
#include "json.hpp"
#include "Boulder.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

using namespace cocos2d;
using namespace JTTW;

const int UI_LAYER_Z_IDX = 2;
const int LVL_LAYER_Z_IDX = 1;

cocos2d::Scene* MainGameScene::createScene(std::string levelToLoad) {
    // 'scene' and layer are autorelease objects.
    auto scene = cocos2d::Scene::createWithPhysics();
    scene->getPhysicsWorld()->setGravity(cocos2d::Vec2(0, -498));
    scene->getPhysicsWorld()->setDebugDrawMask(cocos2d::PhysicsWorld::DEBUGDRAW_ALL);
    auto layer = MainGameScene::create(levelToLoad, scene->getPhysicsWorld());
    if (layer == NULL) {
        return NULL;
    }
    scene->addChild(layer);

    return scene;
}

bool MainGameScene::onContactHandler(cocos2d::PhysicsContact& contact, bool begin) {
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    cocos2d::Vec2 normal = contact.getContactData()->normal;
    if (nodeA->getTag() == CHARACTER_TAG) {
        Character *c = (Character *)nodeA;
        // If A is the character, then for landing on a flat platform, the normal is 0, -1.
        if (nodeB->getTag() == VINE_TAG && c->characterName == "Monkey") {
            Vine *v = (Vine *)nodeB;
            Monkey *m = (Monkey *)c;
            if (begin) {
                 std::cout << "Collided with a vine" << std::endl;
                 cocos2d::Vec2 nodePoint = m->convertToNodeSpace(contact.getContactData()->points[0]);
                 cocos2d::Vec2 collisionPosition = m->getPosition(); // + nodePoint;
                 float distFromCenter = collisionPosition.distance(v->getPosition());
                 float distFromRotCenter = collisionPosition.distance(v->getRotationCenter());
                 float halfLength = v->getRotationCenter().distance(v->getPosition());
                 float offset;
                 if (distFromRotCenter > halfLength && distFromCenter < halfLength) {
                     offset = -distFromCenter;
                 } else if (distFromRotCenter < halfLength && distFromCenter < halfLength) {
                     offset = distFromCenter;
                 } else {
                     // IDK what's happening?
                     std::cout << "WEIRDNESS HERE!" << std::endl;
                     std::cout << "distFromRotCenter: " << distFromRotCenter << std::endl;
                     std::cout << "distFromCenter: " << distFromCenter << std::endl;
                     std::cout << "halfLength: " << halfLength << std::endl;
                     offset = -(halfLength - 5);
                 }
                 m->enteringVine(this->getScene()->getPhysicsWorld(), v->getPhysicsBody(), offset,  m->getPosition() + cocos2d::Vec2(0.0, m->getContentSize().height));
            } else {
                //m->leavingClimeable();
                return true; // don't rebalance impulse!
            }
        }
        if (normal.dot(cocos2d::Vec2(0, -1)) > std::cos(M_PI/4.0)) {
            if (begin) {
                c->landedCallback(contact.getShapeB()->getBody());
            } else { // onContactEnd
                c->leftCallback(contact.getShapeB()->getBody());
            }
        } else if (nodeB->getTag() == CLIMBEABLE_TAG && c->characterName == "Monkey") {
            // TODO: hacky, fix
            std::cout << "Hit climeable" << std::endl;
            Monkey *m = (Monkey *)c;
            if (begin) {
                m->enteringClimeable();
            } else {
                m->leavingClimeable();
            }
        }
        if (!begin) {
            c->rebalanceImpulse();
        }
        if (!begin) {
            c->rebalanceImpulse();
        }
    }
    
    if (nodeB->getTag() == CHARACTER_TAG) {
        Character *c = (Character *)nodeB;
        // If B is the character, then for landing on a flat platform, the normal is 0, 1.
        if (nodeA->getTag() == VINE_TAG && c->characterName == "Monkey") {
            Vine *v = (Vine *)nodeA;
            Monkey *m = (Monkey *)c;
            if (begin) {
                 std::cout << "Collided with a vine" << std::endl;
                 cocos2d::Vec2 nodePoint = m->convertToNodeSpace(contact.getContactData()->points[0]);
                 cocos2d::Vec2 collisionPosition = m->getPosition(); // + nodePoint;
                 float distFromCenter = collisionPosition.distance(v->getPosition());
                 float distFromRotCenter = collisionPosition.distance(v->getRotationCenter());
                 float halfLength = v->getRotationCenter().distance(v->getPosition());
                 float offset;
                 if (distFromRotCenter > halfLength && distFromCenter < halfLength) {
                     offset = -distFromCenter;
                 } else if (distFromRotCenter < halfLength && distFromCenter < halfLength) {
                     offset = distFromCenter;
                 } else {
                     // IDK what's happening?
                     std::cout << "WEIRDNESS HERE!" << std::endl;
                     std::cout << "distFromRotCenter: " << distFromRotCenter << std::endl;
                     std::cout << "distFromCenter: " << distFromCenter << std::endl;
                     std::cout << "halfLength: " << halfLength << std::endl;
                     offset = -(halfLength - 5);
                 }
                 m->enteringVine(this->getScene()->getPhysicsWorld(), v->getPhysicsBody(), offset,  m->getPosition() + cocos2d::Vec2(0.0, m->getContentSize().height));
            } else {
                //m->leavingClimeable();
                return true; // don't rebalance impulse!
            }
        } else if (normal.dot(cocos2d::Vec2(0, 1)) > std::cos(M_PI /4.0)) {
            if (begin) {
                c->landedCallback(contact.getShapeA()->getBody());
            } else { // onContactEnd
                c->leftCallback(contact.getShapeA()->getBody());
            }
        } else if (nodeA->getTag() == CLIMBEABLE_TAG && c->characterName == "Monkey") {
            // TODO: Hacky, fix.
            std::cout << "Hit climeable" << std::endl;
            Monkey *m = (Monkey *)c;
            if (begin) {
                m->enteringClimeable();
            } else {
                m->leavingClimeable();
            }
        }
        if (!begin) {
            c->rebalanceImpulse();
        }
    }
    return true;
}

bool MainGameScene::onContactBegin(cocos2d::PhysicsContact& contact) {
    return onContactHandler(contact, true);
}

bool MainGameScene::onContactEnd(cocos2d::PhysicsContact& contact) {
    return onContactHandler(contact, false);
}

const int PLATFORM_Z = 4;
const int VINE_Z = 3;

cocos2d::Layer *MainGameScene::parseLevelFromJson(std::string fileName, bool debugOn) {
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    
    cocos2d::Layer *levelLayer = cocos2d::Layer::create();
    cocos2d::Layer *uiLayer = cocos2d::Layer::create();
    
    std::ifstream inFile(fileName);
    nlohmann::json lvl;
    
    std::cout.flush();
    inFile >> lvl;
    
    //vp.setRatio(lvl["mToPixel"]);
    
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
            double centerBX = vp.metersToPixels((double)pAtt["endX"]);
            double centerBY = vp.metersToPixels((double)pAtt["endY"]);
            cocos2d::Vec2 centerB(centerBX, centerBY);
            double maximumVelocity = vp.metersToPixels((double)pAtt["velocity"]);
            MoveablePlatform *p = new MoveablePlatform(fullImagePath, centerA, centerB, cocos2d::Size(imageSizeWidth, imageSizeHeight), cocos2d::Vec2(collisionWidth, collisionHeight), maximumVelocity);

            levelLayer->addChild(p, PLATFORM_Z);
            platforms.push_back(p);
            moveables.push_back(p);
        } else {
            Platform *p = new Platform(fullImagePath, cocos2d::Vec2(centerX, centerY), cocos2d::Size(imageSizeWidth, imageSizeHeight), cocos2d::Vec2(collisionWidth, collisionHeight), pAtt["climbable"]);
        
            levelLayer->addChild(p, PLATFORM_Z);
            platforms.push_back(p);
        }
    }
    nlohmann::json characterStruct = lvl["characters"];
    std::vector<std::string> charNames = {"Monkey", "Monk", "Piggy", "Sandy"};
    int characterHeight = vp.metersToPixels(1.7);
    int characterWidth = vp.metersToPixels(1.7);
    
    cocos2d::Vec2 uiHeadLocation(40, 40);
    for (int i = 0; i < 4; i++) {
        if (characterStruct[charNames[i]]["present"]) {
            double startX = vp.metersToPixels((double)characterStruct[charNames[i]]["startingXPos"]);
            double startY = vp.metersToPixels((double)characterStruct[charNames[i]]["startingYPos"]);

            Character *c = Character::createFromName(charNames[i], cocos2d::Vec2(startX, startY), cocos2d::Size(characterWidth, characterHeight));
            characters.push_back(c);
            levelLayer->addChild(c, i);
            AiAgent *agent = new AiAgent(c);
            agent->setPlayerPosOffset(c->getPosition() - characters[0]->getPosition());
            agents.push_back(agent);
            
            // Set the ui head.
            std::stringstream ss;
            ss << charNames[i] << "Head.png";
            cocos2d::Sprite *head = cocos2d::Sprite::create(ss.str());

            double headScale = .13 * visibleSize.width / 1024.0;

            head->setScale(headScale);
            head->setPosition(uiHeadLocation);
            uiHeadLocation += cocos2d::Vec2(70, 0);
            uiLayer->addChild(head);
        }
    }
    
    if (agents.size() == 0) {
        // Handle Error!
        std::cout << "You cant have a level with no characters!" << std::endl;
        //menuCloseCallback(nullptr);
        throw std::domain_error("No characters");
    }
    player = agents[0];
    player->_controlledCharacter->currentCrown->setVisible(true);

    nlohmann::json boulders = lvl["interactables"]["boulders"];
    for (auto& bAtt: boulders) {
        std::string type = bAtt["type"];
        std::transform(type.begin(), type.end(), type.begin(), ::toupper);
        if (type == "CIRCLE") {
            Boulder *b = new Boulder(
                    vp.metersToPixels((double)bAtt["radius"]),
                    bAtt["imageName"],
                    vp.metersToPixels(cocos2d::Vec2((double)bAtt["centerX"], (double)bAtt["centerY"])),
                    bAtt["mass"]);
            dynamics.push_back(b);
            levelLayer->addChild(b, 10);
        } else if (type == "POLYGON") {
            std::cout << "Don't support polygonal boulders yet!" << std::endl;
            throw std::invalid_argument("no polygonal boulders yet");
        }
    }
    
    nlohmann::json in_vines = lvl["vines"];
    for (auto& vAtt: in_vines) {
        double width = vp.metersToPixels((double)vAtt["width"]);
        double length = vp.metersToPixels((double)vAtt["length"]);
        cocos2d::Vec2 center = cocos2d::Vec2(vp.metersToPixels((double)vAtt["swingCenterX"]),
                  vp.metersToPixels((double)vAtt["swingCenterY"]));
        Vine *v = new Vine("vine3.png", center, width, length);
        
        cocos2d::PhysicsBody *b = cocos2d::PhysicsBody::createBox(cocos2d::Size(3, 3));
        b->setRotationEnable(false);
        b->setDynamic(false);
        b->setCollisionBitmask((int)CollisionCategory::None);

        cocos2d::Sprite *n = cocos2d::Sprite::create();
        n->setPosition(center);
        n->addComponent(b);

        vines.push_back(v);
        levelLayer->addChild(v, VINE_Z);
        auto j = cocos2d::PhysicsJointPin::construct(v->getBody(), b, cocos2d::Vec2(0, length/2.0), cocos2d::Vec2::ZERO);
        _w->addJoint(j);
        levelLayer->addChild(n, VINE_Z);
    }
    
    nlohmann::json in_traps = lvl["traps"];
    for (auto& tAtt: in_traps) {
        // TODO: Read in density and inner and outer box.
        double wallWidth = vp.metersToPixels((double)tAtt["wallThickness"]);
        double trapWidth = vp.metersToPixels((double)tAtt["trapWidth"]);
        double trapHeight = vp.metersToPixels((double)tAtt["trapHeight"]);
        double offset = vp.metersToPixels((double)tAtt["offset"]);
        cocos2d::Size imgSize(vp.metersToPixels((double)tAtt["imageWidth"]), vp.metersToPixels((double)tAtt["imageHeight"]));
        cocos2d::Vec2 center = vp.metersToPixels(cocos2d::Vec2((double)tAtt["centerX"], (double)tAtt["centerY"]));
        
        std::string imageName = tAtt["imageName"];
        if (imageName == "cage1.png") {
            cocos2d::PhysicsMaterial material = cocos2d::PhysicsMaterial(tAtt["density"], tAtt["bounciness"], tAtt["friction"]);
            Trap* rS = new Trap(imageName, center, material, cocos2d::Size(trapWidth, trapHeight), imgSize, wallWidth, offset);
            trapsToTrigger.push_back(rS);
            levelLayer->addChild(rS, 10);
        }
    }

    levelEndX = lvl["levelEndX"];
    _nextLevel = lvl["nextLevelName"];
    
    // Set the ui layer here.
    this->addChild(uiLayer, UI_LAYER_Z_IDX);
    
    return levelLayer;
}

bool MainGameScene::init(std::string levelToLoad, cocos2d::PhysicsWorld *w) {
    if ( !Layer::init() ) {
        return false;
    }
    _w = w;
    
    // aka window dimensions
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Creates the camera, or viewpoint for this scene.
    // 1.7/130.0 means that 1.7 meters in the game world (average human male height) is represented by 180 pixels on screen.
    vp = Viewpoint(visibleSize, 1.7/130.0);

    cocos2d::Layer *layer;
    try {
        layer = parseLevelFromJson(levelToLoad, debugOn);
    }
    catch (std::domain_error ex) {
        std::cout<< "Json was mal-formed, or expected members were not found, " << ex.what() << std::endl;
        return false;
    } catch (std::invalid_argument ex) {
        std::cout<< "Json was mal-formed, or expected members were not found, " << ex.what() << std::endl;
        return false;
    }
 
    if (layer == nullptr) {
        std::cout << "Level file corrupted!" << std::endl;
        return false;
    }
 
    vp.setLayer(layer);

    this->addChild(layer, LVL_LAYER_Z_IDX);
    
    eventListener = cocos2d::EventListenerKeyboard::create();
    eventListener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) mutable {
        afterFirstClick = true;
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
            case EventKeyboard::KeyCode::KEY_UP_ARROW:
            case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
            case EventKeyboard::KeyCode::KEY_SPACE:
            case EventKeyboard::KeyCode::KEY_A:
            case EventKeyboard::KeyCode::KEY_O:
            case EventKeyboard::KeyCode::KEY_S:
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
            if (afterFirstClick) {
                player->plan(characters, keyCode, false);
            }
    };

    this->_eventDispatcher->addEventListenerWithFixedPriority(eventListener, 1);
    
    auto contactListener = cocos2d::EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(MainGameScene::onContactBegin, this);
    contactListener->onContactSeparate = CC_CALLBACK_1(MainGameScene::onContactEnd, this);
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    this->scheduleUpdate();
    return true;
}

void MainGameScene::switchToCharacter(int charIndex) {
    if (charIndex > agents.size() - 1) {
        return;
    }
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

void MainGameScene::menuCloseCallback(Ref* pSender) {
    auto startScene = MainMenu::createScene();
    this->_eventDispatcher->removeEventListener(eventListener);
    cocos2d::Director::getInstance()->replaceScene(startScene);
}

void MainGameScene::nextLevelCallback() {
    if (!nextLevelStarting) {
        this->_eventDispatcher->removeEventListener(eventListener);
        std::cout << "Starting next level, " << _nextLevel << std::endl;
        auto end = LevelEnd::createScene(_nextLevel);
        auto fade = cocos2d::TransitionFade::create(3.0, end);
        cocos2d::Director::getInstance()->replaceScene(fade);
        nextLevelStarting = true;
    }
}

void MainGameScene::update(float delta) {
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
            done = false; // don't go to the next level if all characters die at once!
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
    
    for (int i = 0; i < trapsToTrigger.size(); i++) {
        for (int j = 0; j < characters.size(); j++) {
            trapsToTrigger[i]->triggerIfUnder(characters[j]->getPosition());
        }
    }

    vp.followCharacter(player->_controlledCharacter, delta);
}
