#include "MainGameScene.h"
#include "SimpleAudioEngine.h"
#include "MainMenuScene.hpp"
#include "Collisions.hpp"
#include <iostream>
#include "Character.hpp"
#include "Monkey.hpp"
#include "LevelEnd.hpp"
#include "Platform.hpp"
#include "Vine.hpp"
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
    scene->getPhysicsWorld()->setGravity(cocos2d::Vec2(0, -298));
    scene->getPhysicsWorld()->setDebugDrawMask(cocos2d::PhysicsWorld::DEBUGDRAW_ALL);
    auto layer = HelloWorld::create(levelToLoad, scene->getPhysicsWorld());
    if (layer == NULL) {
        return NULL;
    }
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
                //m->leavingClimeable();
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
                m->leavingClimeable();
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

bool HelloWorld::onContactBegin(cocos2d::PhysicsContact& contact) {
    return onContactHandler(contact, true);
}

bool HelloWorld::onContactEnd(cocos2d::PhysicsContact& contact) {
    return onContactHandler(contact, false);
}

cocos2d::Layer *HelloWorld::parseLevelFromJson(std::string fileName, bool debugOn) {
    
    cocos2d::Layer *levelLayer = cocos2d::Layer::create();
    
    const int platformZ = 4;
    
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

            levelLayer->addChild(p->image, platformZ);
            platforms.push_back(p);
            moveables.push_back(p);
        } else {
            Platform *p = new Platform(fullImagePath, cocos2d::Vec2(centerX, centerY), cocos2d::Size(imageSizeWidth, imageSizeHeight), cocos2d::Vec2(collisionWidth, collisionHeight), pAtt["climbable"]);
        
            levelLayer->addChild(p->getImage(), platformZ);
            platforms.push_back(p);
        }
    }
    nlohmann::json characterStruct = lvl["characters"];
    std::vector<std::string> charNames = {"Monkey", "Monk", "Piggy", "Sandy"};
    int characterHeight = vp.metersToPixels(1.7);
    int characterWidth = vp.metersToPixels(1.7);
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
        std::cout << "Reading in boulders" << std::endl;
        cocos2d::PhysicsBody *body = cocos2d::PhysicsBody::createCircle(vp.metersToPixels((double)bAtt["radius"]));
        body->setCategoryBitmask((int)CollisionCategory::Boulder);
        body->setCollisionBitmask((int)CollisionCategory::ALL);
        body->setContactTestBitmask((int)CollisionCategory::ALL);
        body->setRotationEnable(true);
        
        body->setVelocityLimit(600);
        body->setDynamic(true);
        
        cocos2d::Sprite * bSprite = cocos2d::Sprite::create("Sandy2.png");
        bSprite->setPosition(vp.metersToPixels((double)bAtt["centerX"]), vp.metersToPixels((double)bAtt["centerY"]));
        bSprite->addComponent(body);
        dynamics.push_back(bSprite);
        
        levelLayer->addChild(bSprite, 10);
        
        std::cout << "Added boulder to level" << std::endl;
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

        cocos2d::Sprite *n = cocos2d::Sprite::create("cage1.png");
        n->setContentSize(cocos2d::Size(10, 10));
        n->setPosition(center);
        n->addComponent(b);

        vines.push_back(v);
        levelLayer->addChild(v, 10);
        auto j = cocos2d::PhysicsJointPin::construct(v->getBody(), b, cocos2d::Vec2(0, length/2.0), cocos2d::Vec2::ZERO);
        _w->addJoint(j);
        levelLayer->addChild(n, 10);
    }
    
    nlohmann::json in_traps = lvl["traps"];
    for (auto& tAtt: in_traps) {
        cocos2d::PhysicsShapeBox *l = cocos2d::PhysicsShapeBox::create(cocos2d::Size(30, 200), cocos2d::PhysicsMaterial(2.0, 0.1, 1.0), cocos2d::Vec2(-110, 0));
        cocos2d::PhysicsShapeBox *r = cocos2d::PhysicsShapeBox::create(cocos2d::Size(30, 200),
            cocos2d::PhysicsMaterial(2.0, 0.1, 1.0), cocos2d::Vec2(110, 0));
        cocos2d::PhysicsShapeBox *t = cocos2d::PhysicsShapeBox::create(cocos2d::Size(220, 30),
            cocos2d::PhysicsMaterial(2.0, 0.1, 1.0), cocos2d::Vec2(0, 100));
        cocos2d::PhysicsBody *body = cocos2d::PhysicsBody::create();
        body->addShape(l);
        body->addShape(r);
        body->addShape(t);
        //cocos2d::PhysicsBody *bodyR = cocos2d::PhysicsBody::createBox(cocos2d::Size(30, 150));
        //cocos2d::PhysicsBody *bodyTop = cocos2d::PhysicsBody::createBox(cocos2d::Size(150, 30));
        
        body->setCategoryBitmask((int)CollisionCategory::Boulder);
        body->setCollisionBitmask((int)CollisionCategory::ALL);
        body->setContactTestBitmask((int)CollisionCategory::ALL);
        body->setRotationEnable(true);
        
        body->setVelocityLimit(600);
        body->setDynamic(true);
        body->setPositionOffset(cocos2d::Vec2(0, -40));
        
        //bodyR->setCategoryBitmask((int)CollisionCategory::Boulder);
        //bodyR->setCollisionBitmask((int)CollisionCategory::ALL);
        //bodyR->setContactTestBitmask((int)CollisionCategory::ALL);
        //bodyR->setRotationEnable(true);
        
        //bodyR->setVelocityLimit(600);
        //bodyR->setDynamic(true);
        
        //bodyTop->setCategoryBitmask((int)CollisionCategory::Boulder);
        //bodyTop->setCollisionBitmask((int)CollisionCategory::ALL);
        //bodyTop->setContactTestBitmask((int)CollisionCategory::ALL);
        //bodyTop->setRotationEnable(true);
        
        //bodyTop->setVelocityLimit(600);
        //bodyTop->setDynamic(true);
        
        //cocos2d::PhysicsJoint* j1 = cocos2d::PhysicsJointFixed::construct(bodyL, bodyTop, cocos2d::Vec2::ANCHOR_MIDDLE_TOP);
        
        //cocos2d::PhysicsJoint* j2 = cocos2d::PhysicsJointFixed::construct(bodyTop, bodyR, cocos2d::Vec2::ANCHOR_BOTTOM_RIGHT);
        //j2->createConstraints();
        //j1->createConstraints();
        //j1->setDistance(0.0);
        //j2->setDistance(0.0);
        //j2->createConstraints();
        //j1->createConstraints();
        
        //cocos2d::Sprite * lSprite = cocos2d::Sprite::create("Sandy2.png");
        //lSprite->setPosition(vp.metersToPixels((double)tAtt["centerX"]), vp.metersToPixels((double)tAtt["centerY"]) + 75);
        //lSprite->setContentSize(cocos2d::Size(30, 150));
        //lSprite->addComponent(bodyL);
        
        cocos2d::Sprite * rS = cocos2d::Sprite::create("cage1.png");
        rS->setPosition(vp.metersToPixels((double)tAtt["centerX"]), vp.metersToPixels((double)tAtt["centerY"]));
        rS->setContentSize(cocos2d::Size(200, 280));
        rS->addComponent(body);
        
        //cocos2d::Sprite * tSprite = cocos2d::Sprite::create("Sandy2.png");
        //tSprite->setPosition(vp.metersToPixels((double)tAtt["centerX"] - 75), vp.metersToPixels((double)tAtt["centerY"]));
        //tSprite->setContentSize(cocos2d::Size(30, 150));
        //tSprite->addComponent(bodyTop);
   
        dynamics.push_back(rS);
        //dynamics.push_back(rSprite);
        //dynamics.push_back(tSprite);
        
        levelLayer->addChild(rS, 10);
        //levelLayer->addChild(rSprite, 10);
        //levelLayer->addChild(tSprite, 10);
        
        //_w->addJoint(j1);
        //_w->addJoint(j2);
    }

    levelEndX = lvl["levelEndX"];
    _nextLevel = lvl["nextLevelName"];
    
    
    
    return levelLayer;
}

bool HelloWorld::init(std::string levelToLoad, cocos2d::PhysicsWorld *w) {
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
    } catch (std::domain_error ex) {
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

    this->addChild(layer, 1);
    
    eventListener = cocos2d::EventListenerKeyboard::create();
    eventListener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) mutable {
        firstClick = true;
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
            if (firstClick) {
                player->plan(characters, keyCode, false);
            }
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
    
    for (int i = 0; i < vines.size(); i++) {
        vines[i]->move();
    }
    vp.followCharacter(player->_controlledCharacter, delta);
}
