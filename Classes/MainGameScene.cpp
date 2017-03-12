#include "MainGameScene.h"
#include "SimpleAudioEngine.h"
#include "MainMenuScene.hpp"
#include "DisappearingPlatform.hpp"
#include "Collisions.hpp"
#include "Monkey.hpp"
#include "Monk.hpp"
#include "LevelEnd.hpp"
#include "cocos2d.h"
#include "json.hpp"
#include "Boulder.hpp"
#include "CageTrap.hpp"
#include "Spear.hpp"
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

bool MainGameScene::characterCollision(cocos2d::PhysicsContact& contact, bool begin, Character *c, cocos2d::PhysicsBody *body, Node *node, cocos2d::Vec2 normal) {
    if (node->getTag() == VINE_TAG && c->characterName == "Monkey") {
        Vine *v = (Vine *)node;
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
             m->enteringVine(this->getScene()->getPhysicsWorld(), v, offset, m->getPosition() + cocos2d::Vec2(0.0, m->getContentSize().height), false);
        } else {
            //m->leavingClimeable();
            return true; // don't rebalance impulse!
        }
    }
    if (node->getTag() == PROJECTILE_TAG) {
        c->restartFromRespawn();
        return true;
    }
    if (node->getTag() == PEG_TAG && c->characterName == "Monk") {
        std::cout << "Hit Peg!" << std::endl;
        // Add peg to Monk's peg hitting thing.
        Monk *m = (Monk *)c;
        m->addReachiblePeg((Peg *)node);
        return false;
    }
    if (normal.dot(cocos2d::Vec2(0, -1)) > std::cos(M_PI / 4.0)) {
        if (begin) {
            c->landedCallback(body, normal.getPerp());
        } else { // onContactEnd
            c->leftCallback(body);
        }
    } else if (node->getTag() == CLIMBEABLE_TAG && c->characterName == "Monkey") {
        // TODO: hacky, fix
        Monkey *m = (Monkey *)c;
        if (begin) {
            m->enteringClimeable(normal.getPerp());
        } else {
            m->leavingClimeable();
        }
    }
    if (!begin) {
        c->rebalanceImpulse();
    }
    return true;
}

bool MainGameScene::onContactHandler(cocos2d::PhysicsContact& contact, bool begin) {
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    auto normal = contact.getContactData()->normal;
    if (nodeA->getTag() == CHARACTER_TAG) {
        Character *c = (Character *)nodeA;
        // If A is the character, then for landing on a flat platform, the normal is 0, -1.
        return characterCollision(contact, begin, c, contact.getShapeB()->getBody(), nodeB, normal);
    }
    
    if (nodeB->getTag() == CHARACTER_TAG) {
        Character *c = (Character *)nodeB;
        // If B is the character, then for landing on a flat platform, the normal is 0, 1.
        return characterCollision(contact, begin, c, contact.getShapeA()->getBody(), nodeA, -normal);
    }
    
    if (begin && (nodeA->getTag() == PROJECTILE_TAG || nodeB->getTag() == PROJECTILE_TAG)) {
        return false; // don't collide with anything.
    }
    
    return true;
}

bool MainGameScene::onContactBegin(cocos2d::PhysicsContact& contact) {
    //std::cout << "Is it boulder?" << std::endl;
    return onContactHandler(contact, true);
}

bool MainGameScene::onContactEnd(cocos2d::PhysicsContact& contact) {
    return onContactHandler(contact, false);
}

const int PLATFORM_Z = 4;
const int FLOOR_Z = 7;
const int CLIMBABLE_Z = 3;
const int VINE_Z = 3;
const int BOULDER_Z = 5;
const int CHARACTER_Z = 6;

cocos2d::Layer *MainGameScene::parseLevelFromJson(std::string fileName, bool debugOn) {
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    
    cocos2d::Layer *levelLayer = cocos2d::Layer::create();
    cocos2d::Layer *uiLayer = cocos2d::Layer::create();
    
    std::ifstream inFile(fileName);
    nlohmann::json lvl;
    
    inFile >> lvl;

    // draw and add background
    nlohmann::json backgroundAtts = lvl["background"];
    std::string bgPath = backgroundAtts["imageName"];
    bgPath = "assets/" + bgPath;
    auto background = cocos2d::Sprite::create(bgPath);
    background->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    background->setScale(1.4);
    background->setPosition(0,-300.0);
    this->addChild(background, -8);
    
    nlohmann::json platformAtts = lvl["platforms"];
    for (auto& pAtt: platformAtts) {
        std::string fullImagePath = pAtt["imageName"];
        fullImagePath = "assets/" + fullImagePath;
        double centerX = vp.metersToPixels((double)pAtt["centerX"]);
        double centerY = vp.metersToPixels((double)pAtt["centerY"]);
        double imageSizeWidth = vp.metersToPixels((double)pAtt["imageSizeWidth"]);
        double imageSizeHeight = vp.metersToPixels((double)pAtt["imageSizeHeight"]);
        
        std::vector<cocos2d::Vec2> ps;
        for (auto& cPoint: pAtt["collisionPoints"]) {
            ps.push_back(cocos2d::Vec2(vp.metersToPixels((double)cPoint["x"]), vp.metersToPixels((double)cPoint["y"])));
        }
        if (!pAtt["moveable"].is_boolean()) {
            // Something is wrong!
            std::cout << "Platform " << fullImagePath << " doesn't have a moveable member." << std::endl;
            throw std::domain_error("No moveable");
        }
        
        if (pAtt["moveable"]) {
            cocos2d::Vec2 centerA(centerX, centerY);
            double centerBX = vp.metersToPixels((double)pAtt["endX"]);
            double centerBY = vp.metersToPixels((double)pAtt["endY"]);
            cocos2d::Vec2 centerB(centerBX, centerBY);
            double maximumVelocity = vp.metersToPixels((double)pAtt["velocity"]);
            MoveablePlatform *p = new MoveablePlatform(fullImagePath, centerA, centerB, cocos2d::Size(imageSizeWidth, imageSizeHeight), ps, maximumVelocity);

            if (fullImagePath == "assets/blueGround.png") {
                std::cout << "Found ground!" << std::endl;
                levelLayer->addChild(p, FLOOR_Z);
            } else {
                std::cout << fullImagePath << std::endl;
                levelLayer->addChild(p, PLATFORM_Z);
            }
            platforms.push_back(p);
            moveables.push_back(p);
        } else {
            if (pAtt["disappears"]) {
                DisappearingPlatform *p = new DisappearingPlatform(fullImagePath, cocos2d::Vec2(centerX, centerY), cocos2d::Size(imageSizeWidth, imageSizeHeight),
                   ps, pAtt["climbable"], pAtt["collidable"]);
                levelLayer->addChild(p, PLATFORM_Z);
                trapsToTrigger.push_back(p);
            } else {
                Platform *p = new Platform(fullImagePath, cocos2d::Vec2(centerX, centerY), cocos2d::Size(imageSizeWidth, imageSizeHeight), ps, pAtt["climbable"], pAtt["collidable"]);
        
                if (p->getTag() == CLIMBEABLE_TAG) {
                    levelLayer->addChild(p, CLIMBABLE_Z);
                } else if (fullImagePath == "assets/blueGround.png") {
                    levelLayer->addChild(p, FLOOR_Z);
                } else {
                    levelLayer->addChild(p, PLATFORM_Z);
                }
                platforms.push_back(p);
            }
        }
    }
    nlohmann::json characterStruct = lvl["characters"];
    std::vector<std::string> charNames = {"Monkey", "Monk", "Piggy", "Sandy"};
    int characterHeight = vp.metersToPixels(1.7);
    int characterWidth = vp.metersToPixels(1.7);
    
    cocos2d::Vec2 uiHeadLocation(40, 40);
    int charPresentCount = 0;
    for (int i = 0; i < 4; i++) {
        if (characterStruct[charNames[i]]["present"]) {
            double startX = vp.metersToPixels((double)characterStruct[charNames[i]]["startingXPos"]);
            double startY = vp.metersToPixels((double)characterStruct[charNames[i]]["startingYPos"]);

            Character *c = Character::createFromName(charNames[i], cocos2d::Vec2(startX, startY), cocos2d::Size(characterWidth, characterHeight));
            characters.push_back(c);
            levelLayer->addChild(c, CHARACTER_Z);
            AiAgent *agent = new AiAgent(c);
            agent->setPlayerPosOffset(c->getPosition() - characters[0]->getPosition());
            agents.push_back(agent);
            
            // Set the ui head.
            std::stringstream ss;
            ss << "characters/" << charNames[i] << "Head.png";
            cocos2d::Sprite *head = cocos2d::Sprite::create(ss.str());
            
            double headScale = .13 * visibleSize.width / 1024.0;

            head->setScale(headScale);
            head->setPosition(uiHeadLocation + (charPresentCount * cocos2d::Vec2(70, 0)));
 
            
            std::string buttonString;
            switch(charPresentCount) {
                case 0:
                    buttonString = "z";
                    break;
                case 1:
                    buttonString = "x";
                    break;
                case 2:
                    buttonString = "c";
                    break;
                case 3:
                    buttonString = "v";
                    break;
            }
            auto label = cocos2d::Label::createWithTTF(buttonString, "fonts/WaitingfortheSunrise.ttf", 40);
            label->setTextColor(cocos2d::Color4B::WHITE);
            label->enableOutline(cocos2d::Color4B::BLACK, 1);
            label->enableShadow();
            label->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
            label->setPosition(uiHeadLocation + (charPresentCount * cocos2d::Vec2(70, 0)) + cocos2d::Vec2(0, 70));
            uiLayer->addChild(label, 10);
            
            //uiHeadLocation += cocos2d::Vec2(70, 0);
            uiLayer->addChild(head);
            charPresentCount++;
        }
    }
    
    if (agents.size() == 0) {
        // Handle Error!
        std::cout << "You cant have a level with no characters!" << std::endl;
        throw std::domain_error("No characters");
    }
    player = agents[0];
    player->_controlledCharacter->currentCrown->setVisible(true);

    nlohmann::json boulderJson = lvl["boulders"];
    for (auto& bAtt: boulderJson) {
        cocos2d::Vec2 center = vp.metersToPixels(cocos2d::Vec2((double)bAtt["centerX"], (double)bAtt["centerY"]));
        std::stringstream ss;
        std::string temp = bAtt["imageName"];
        ss << "assets/" << temp;
        std::string imageName = ss.str();
        double mass = bAtt["mass"];
        std::string type = bAtt["type"];
        cocos2d::Size imgSize = cocos2d::Size(vp.metersToPixels((double)bAtt["imageSizeWidth"]), vp.metersToPixels((double)bAtt["imageSizeHeight"]));
        std::transform(type.begin(), type.end(), type.begin(), ::toupper);
        Boulder *b;
        if (type == "CIRCLE") {
            b = new Boulder(
                    vp.metersToPixels((double)bAtt["radius"]),
                    imageName,
                    center,
                    mass,
                    imgSize);
        } else if (type == "POLYGON") {
            std::vector<cocos2d::Vec2> points;
            for (auto& point: bAtt["collisionPoints"]) {
                points.push_back(vp.metersToPixels(cocos2d::Vec2((double)point["x"], (double)point["y"])));
            }
            b = new Boulder(points, imageName, center, mass, imgSize);
        } else {
            std::cout << "Boulder type should be either CIRCLE or POLYGON, not " << type << std::endl;
            throw std::invalid_argument("Boulder type should be circle or polygon");
        }
        boulders[(int)bAtt["ticket"]] = b;
        levelLayer->addChild(b, BOULDER_Z);
    }
    
    nlohmann::json boulderJoints = lvl["boulderJoints"];
    for (auto&jAtt : boulderJoints) {
        int id1 = jAtt["id1"];
        int id2 = jAtt["id2"];
        Boulder *b1 = boulders[id1];
        Boulder *b2 = boulders[id2];
        //b1->getBody()->setCollisionBitmask((int)CollisionCategory::CharacterAndPlatform);
        //b2->getBody()->setCollisionBitmask((int)CollisionCategory::CharacterAndPlatform);
        //b1->getBody()->setContactTestBitmask((int)CollisionCategory::CharacterAndPlatform);
        //b2->getBody()->setContactTestBitmask((int)CollisionCategory::CharacterAndPlatform);
        b1->getBody()->setDynamic(false);
        b2->getBody()->setDynamic(false);
        
        cocos2d::Vec2 offset1 = vp.metersToPixels(cocos2d::Vec2((double)jAtt["anchor1x"], (double)jAtt["anchor1y"]));
        cocos2d::Vec2 offset2 = vp.metersToPixels(cocos2d::Vec2((double)jAtt["anchor2x"], (double)jAtt["anchor2y"]));
        
        joints[(int)jAtt["jointID"]] = {id1, id2};
        
        //auto pin = cocos2d::PhysicsJointPin::construct(b1->getBody(), b2->getBody(), offset1, offset2);
        //auto gear = cocos2d::PhysicsJointGear::construct(b1->getBody(), b2->getBody(), 0, 1.0);
        //_w->addJoint(pin);
        //_w->addJoint(gear);
    }
    
    nlohmann::json inputpegs = lvl["goldenPegs"];
    for (auto& gAtt: inputpegs) {
        std::cout << gAtt << std::endl;
        std::cout << gAtt["imageWidth"] << std::endl;
        if (gAtt.is_null() || gAtt["imageWidth"].is_null()) {
            std::cout << "WTF IS HAPPENING?" << std::endl;
            continue;
        }
        if (!gAtt["imageName"].is_string()) {
            throw std::domain_error("Golden peg doesn't have an image name!");
        }
        std::string imageName = gAtt["imageName"];
        imageName = "assets/" + imageName;
        cocos2d::Size imageSize = cocos2d::Size(vp.metersToPixels((double)gAtt["imageWidth"]), vp.metersToPixels((double)gAtt["imageHeight"]));
        cocos2d::Vec2 center = vp.metersToPixels(cocos2d::Vec2((double)gAtt["centerX"], (double)gAtt["centerY"]));
        double rotation = 180 * (double)gAtt["rotation"] / 3.1415926;
        std::vector<Boulder *> bouldersToRelease = std::vector<Boulder *>();
        for (auto i: gAtt["bouldersAffected"]) {//joints[(int)gAtt["jointID"]]) {
            bouldersToRelease.push_back(boulders[i]);
        }
        auto peg = new Peg(imageName, center, imageSize, rotation, bouldersToRelease);
        pegs.push_back(peg);
        levelLayer->addChild(peg);
        
        // TODO: MOVE THIS ELSEWHERE.
        Monkey *m = (Monkey *)characters[0];
        m->body->setDynamic(false);
        m->body->setGravityEnable(false);
        m->body->setRotationEnable(false);
        m->freeze();
        m->findSlot("Body")->a = 0.0;
        m->findSlot("L Arm")->a = 0.0;
        m->findSlot("L Calf")->a = 0.0;
        m->findSlot("L Foot")->a = 0.0;
        m->findSlot("L Thigh")->a = 0.0;
        m->findSlot("R Calf")->a = 0.0;
        m->findSlot("R Foot")->a = 0.0;
        m->findSlot("R Thigh")->a = 0.0;
        m->findSlot("R Arm")->a = 0.0;
    }
    
    nlohmann::json in_vines = lvl["vines"];
    for (auto& vAtt: in_vines) {
        double width = vp.metersToPixels((double)vAtt["width"]);
        double length = vp.metersToPixels((double)vAtt["length"]);
        double startingAngVel = vAtt["startingVelocity"];
        cocos2d::Vec2 center = cocos2d::Vec2(vp.metersToPixels((double)vAtt["swingCenterX"]),
                  vp.metersToPixels((double)vAtt["swingCenterY"]));
        std::string imageName = vAtt["imageName"];
        imageName = "assets/" + imageName;
        Vine *v = new Vine(imageName, center, width, length, startingAngVel);
        
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
            imageName = "assets/" + imageName;
            CageTrap *rS = new CageTrap(imageName, center, material, cocos2d::Size(trapWidth, trapHeight), imgSize, wallWidth, offset);
            trapsToTrigger.push_back(rS);
            levelLayer->addChild(rS, 10);
        }
    }
    
    if (lvl["attackZones"].is_array()) {
        nlohmann::json zones = lvl["attackZones"];
        for (auto& zAtt : zones) {
            std::cout << "Reading zone" << std::endl;
            attackZones.push_back(Zone(
                    vp.metersToPixels(cocos2d::Vec2((double)zAtt["minX"], (double)zAtt["minY"])),
                    vp.metersToPixels(cocos2d::Vec2((double)zAtt["maxX"], (double)zAtt["maxY"]))));
        }
    } else {
        std::cout << "Note: no zones in this level." << std::endl;
    }
    
    if (lvl["tutorials"].is_array()) {
        nlohmann::json in_tutorials = lvl["tutorials"];
        for (auto &tAtt : in_tutorials) {
            std::string tipString = tAtt["tipString"];
            int fontSize = tAtt["fontSize"];
            cocos2d::Vec2 center = vp.metersToPixels(cocos2d::Vec2((double)tAtt["centerX"], (double)tAtt["centerY"]));
            auto label = cocos2d::Label::createWithTTF(tipString, "fonts/WaitingfortheSunrise.ttf", fontSize);
            label->setTextColor(cocos2d::Color4B::WHITE);
            label->enableOutline(cocos2d::Color4B::BLACK, 1);
            label->enableShadow();
            label->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
            label->setPosition(center);
            levelLayer->addChild(label, 10);
        }
    }

    levelEndX = vp.metersToPixels((double)lvl["levelEndX"]);
    _nextLevel = lvl["nextLevelName"];
    nlohmann::json respawns = lvl["respawnPoints"];
    for (auto& rps: respawns) {
        double x = vp.metersToPixels((double)rps["x"]);
        double y = vp.metersToPixels((double)rps["y"]);
        respawnPoints.push_back(cocos2d::Vec2(x, y));
    }
    
    // Set the ui layer here.
    this->addChild(uiLayer, UI_LAYER_Z_IDX);
    
    if (lvl["levelName"].is_string()) {
        std::string levelName = lvl["levelName"];
    
        // Audio!
        audio->playBackgroundMusic((std::string("Music/") + levelName + std::string(".mp3")).c_str());
    }
    return levelLayer;
}

bool MainGameScene::init(std::string levelToLoad, cocos2d::PhysicsWorld *w) {
    if ( !Layer::init() ) {
        return false;
    }
    
    _w = w;
    audio = CocosDenshion::SimpleAudioEngine::getInstance();
    
    // aka window dimensions
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Creates the camera, or viewpoint for this scene.
    // 1.7/130.0 means that 1.7 meters in the game world (average human male height) is represented by 180 pixels on screen.
    vp = Viewpoint(visibleSize, 1.7/130.0);

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
                
            case EventKeyboard::KeyCode::KEY_0: {
                // Throw a projectile somewhere!
                Spear *sprite = new Spear(player->_controlledCharacter->getPosition());
                layer->addChild(sprite);
                break;
            }

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
    if (charIndex > (int)agents.size() - 1) {
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
    audio->stopBackgroundMusic();
    audio->preloadBackgroundMusic("Music/MenuScreen.mp3");
    audio->playBackgroundMusic("Music/MenuScreen.mp3");
    auto startScene = MainMenu::createScene();
    this->_eventDispatcher->removeEventListener(eventListener);
    cocos2d::Director::getInstance()->replaceScene(startScene);
}

void MainGameScene::nextLevelCallback() {
    if (!nextLevelStarting) {
        this->_eventDispatcher->removeEventListener(eventListener);
        std::cout << "Starting next level, " << _nextLevel << std::endl;
        audio->stopBackgroundMusic();
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
        } else {
            (*xAgent)->_controlledCharacter->continueMotion();
        }
    }
    
    bool done = true;
    for (int i = 0; i < (int)characters.size(); i++) {
        characters[i]->updateLoop(delta);
        for (auto& zone : attackZones) {
            if (zone.containsPoint(characters[i]->getPosition())) {
                if (attacking[characters[i]] == false) {
                    std::cout << "Attacking " << characters[i]->characterName << "!" << std::endl;
                    attacking[characters[i]] = true;
                    attackCountdown[characters[i]] = 1.0;
                }
            } else {
                attacking[characters[i]] = false;
            }
        }
        for (int j = 0; j < (int)respawnPoints.size(); j++) {
            if (characters[i]->getPosition().x > respawnPoints[j].x && respawnPoints[j].x > characters[i]->getRespawnProgress()) {
                characters[i]->setNewRespawn(respawnPoints[j]);
            } 
        }
        if (characters[i]->getPosition().y < -100) { // TODO: un-hardcode this.
            done = false; // don't go to the next level if all characters die at once!
            characters[i]->restartFromRespawn();
        }  else if (characters[i]->getPosition().x < levelEndX) {
            done = false;
        }
    }
    if (done) {
        for (int i = 0; i < (int)characters.size(); i++) {
            characters[i]->stop();
        }
        nextLevelCallback();
    }
    
    for (auto &m : moveables) {
        m->move(delta, false);
    }
    
    for (auto entry = attacking.begin(); entry != attacking.end(); entry++) {
        if (entry->second) {
             if (attackCountdown[entry->first] <= 0) {
                attackCountdown[entry->first] = 5.0;
                Spear *sprite = new Spear(entry->first->getPosition());
                layer->addChild(sprite);
                break;
            }
            attackCountdown[entry->first] = attackCountdown[entry->first] - delta;
        }
    }
    
    // Trigger any traps, but don't trigger them again.
    std::vector<Trap *> toRemove;
    for (auto &trap : trapsToTrigger) {
        for (auto &character : characters) {
            if (trap->triggerTrap(character->getPosition(), character->getSize())) {
                std::cout << "Trap triggered!" << std::endl;
                switch (trap->characterReaction()) {
                    case FREEZE:
                        character->freeze();
                        break;
                    
                    case FALL:
                        character->setAnimation(0, "fall forwards", false);
                        break;
                    
                    default:
                        // Do nothing.
                        break;
                }
                // remove the trap from the array.
                toRemove.push_back(trap);
            }
        }
    }

    for (auto trap = toRemove.begin(); trap != toRemove.end(); trap++) {
        trapsToTrigger.erase(std::remove(trapsToTrigger.begin(), trapsToTrigger.end(), *trap), trapsToTrigger.end());
    }
    
    if (pegs.size() != 0) {
        bool allTriggered = true;
        for (auto &p: pegs) {
            if (!p->isTriggered()) {
                allTriggered = false;
                break;
            }
        }
        
        if (allTriggered) {
            std::cout << "All pegs triggered!" << std::endl;
            pegs.clear();
            
            Monkey *m = (Monkey *)characters[0];
            layer->removeChild(m);
            m->body->setDynamic(true);
            m->body->setGravityEnable(true);
            m->body->setRotationEnable(false);
            m->setRotation(0.0);
            m->findSlot("Body")->a = 255.0;
            m->findSlot("L Arm")->a = 255.0;
            m->findSlot("L Calf")->a = 255.0;
            m->findSlot("L Foot")->a = 255.0;
            m->findSlot("L Thigh")->a = 255.0;
            m->findSlot("R Calf")->a = 255.0;
            m->findSlot("R Foot")->a = 255.0;
            m->findSlot("R Thigh")->a = 255.0;
            m->findSlot("R Arm")->a = 255.0;
            m->update(0);
            m->removeComponent(m->body);
            m->body->removeFromWorld();
            m->body = cocos2d::PhysicsBody::create();
            auto bodyShape = cocos2d::PhysicsShapeBox::create(cocos2d::Size(480.0f, 780.0f), cocos2d::PhysicsMaterial(1.0, 0.0, 0.0));
            m->body->addShape(bodyShape);
            m->body->setCategoryBitmask((int)CollisionCategory::Character);
            m->body->setCollisionBitmask((int)CollisionCategory::PlatformAndBoulder);
            m->body->setContactTestBitmask((int)CollisionCategory::PlatformBoulderAndProjectile);
            m->body->setRotationEnable(false);

            m->body->setVelocityLimit(600);
    
            m->addComponent(m->body);

            m->setSlotsToSetupPose();
            layer->addChild(m, CHARACTER_Z);
        }
    }

    vp.followCharacter(player->_controlledCharacter, delta);
}
