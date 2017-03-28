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
#include "ProjectileFactory.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

using namespace cocos2d;
using namespace JTTW;

const int UI_LAYER_Z_IDX = 2;
const int LVL_LAYER_Z_IDX = 1;

const int GRAVITY = ideal2Res(-750);
const cocos2d::Vec2 UI_HEAD_START = cocos2d::Vec2(ideal2Res(40.0), ideal2Res(40.0));
const double UI_HEAD_INC = ideal2Res(70);

cocos2d::Scene* MainGameScene::createScene(std::string levelToLoad) {
    // 'scene' and layer are autorelease objects.
    auto scene = cocos2d::Scene::createWithPhysics();
    scene->getPhysicsWorld()->setGravity(cocos2d::Vec2(0, GRAVITY));
    //scene->getPhysicsWorld()->setDebugDrawMask(cocos2d::PhysicsWorld::DEBUGDRAW_ALL);
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
                 offset = -(halfLength - 5);
             }
             m->enteringVine(this->getScene()->getPhysicsWorld(), v, offset, m->getPosition() + cocos2d::Vec2(0.0, m->getContentSize().height), false);
             return true;
        } else {
            //m->leavingClimeable();
            return true; // don't rebalance impulse!
        }
    }
    if (node->getTag() == PROJECTILE_TAG) {
        if (begin) {
            c->setToRespawn();
            return false;
        }
    }
    if (node->getTag() == PEG_TAG && c->characterName == "Monk") {
        std::cout << "Hit Peg!" << std::endl;
        if (begin) {
            // Add peg to Monk's peg hitting thing.
            Monk *m = (Monk *)c;
            m->addReachiblePeg((Peg *)node);
            return false;
        }
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
    } else {
        // Non climbable vertical wall.
        if (begin) {
            c->wallHitCallback(body);
        } else {
            c->wallLeftCallback(body);
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
    return onContactHandler(contact, true);
}

bool MainGameScene::onContactEnd(cocos2d::PhysicsContact& contact) {
    return onContactHandler(contact, false);
}

bool MainGameScene::onContactPostSolve(cocos2d::PhysicsContact& contact) {
    // Set monkey to kinematic?
    return true;
}

const int PLATFORM_Z = 4;
const int VINE_Z = 3;
const int BOULDER_Z = 5;
const int CHARACTER_Z = 6;

cocos2d::Layer *MainGameScene::parseLevelFromJsonV2(nlohmann::json lvl, bool debugOn) {
    cocos2d::Layer *levelLayer = cocos2d::Layer::create();
    uiLayer = cocos2d::Layer::create();
    uiLayer->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    
    // draw and add background
    nlohmann::json backgroundAtts = lvl["bg"];
    std::string bgPath = backgroundAtts["path"];
    bgPath = "assets/" + bgPath;
    auto background = cocos2d::Sprite::create(bgPath);
    background->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    background->setScale(1.4);
    background->setPosition(0,-300.0);
    this->addChild(background, -8);
    
    nlohmann::json platformAtts = lvl["plats"];
    for (auto& pAtt: platformAtts) {
        std::string fullImagePath = pAtt["path"];
        fullImagePath = "assets/" + fullImagePath;
        double centerX = vp.metersToPixels((double)pAtt["centerXM"]);
        double centerY = vp.metersToPixels((double)pAtt["centerYM"]);
        double imageSizeWidth = vp.metersToPixels((double)pAtt["scaledIGWM"]);
        double imageSizeHeight = vp.metersToPixels((double)pAtt["scaledIGHM"]);
        
        int z;
        if (pAtt["book"]["intList"]["Z-order"].is_number_integer()) {
            z = pAtt["book"]["intList"]["Z-order"];
        } else {
            std::cout << "Using default z-order for " << fullImagePath << pAtt["ticket"] << std::endl;
            z = PLATFORM_Z;
        }
        
        std::vector<cocos2d::Vec2> ps;
        for (auto& cPoint: pAtt["book"]["collPointList"]) {
            ps.push_back(cocos2d::Vec2(vp.metersToPixels((double)cPoint["x"]), vp.metersToPixels((double)cPoint["y"])));
        }
        if (!pAtt["book"]["boolList"]["Moving"].is_boolean()) {
            // Something is wrong!
            std::cout << "Platform " << fullImagePath << " doesn't have a moveable member." << std::endl;
            throw std::domain_error("No moveable");
        }
        
        if (pAtt["book"]["boolList"]["Moving"]) {
            cocos2d::Vec2 centerA(centerX, centerY);
            double centerBX = vp.metersToPixels((double)pAtt["endpoint"]["x"]);
            double centerBY = vp.metersToPixels((double)pAtt["endpoint"]["y"]);
            cocos2d::Vec2 centerB(centerBX, centerBY);
            double maximumVelocity = ideal2Res(vp.metersToPixels((double)pAtt["book"]["doubList"]["Velocity"]));
            MoveablePlatform *p = new MoveablePlatform(fullImagePath, centerA, centerB, cocos2d::Size(imageSizeWidth, imageSizeHeight), ps, maximumVelocity);

            std::cout << fullImagePath << std::endl;
            levelLayer->addChild(p, z);
            platforms.push_back(p);
            moveables.push_back(p);
        } else {
            if (pAtt["book"]["boolList"]["Disappears"]) {
                DisappearingPlatform *p = new DisappearingPlatform(fullImagePath, cocos2d::Vec2(centerX, centerY), cocos2d::Size(imageSizeWidth, imageSizeHeight),
                   ps, pAtt["book"]["boolList"]["Climbable"], pAtt["book"]["boolList"]["Collidable"]);
                levelLayer->addChild(p, z);
                trapsToTrigger.push_back(p);
            } else {
                Platform *p = new Platform(fullImagePath, cocos2d::Vec2(centerX, centerY), cocos2d::Size(imageSizeWidth, imageSizeHeight), ps, pAtt["book"]["boolList"]["Climbable"], pAtt["book"]["boolList"]["Collidable"]);
        
                levelLayer->addChild(p, z);
                platforms.push_back(p);
            }
        }
    }
    nlohmann::json characterStruct = lvl["characters"];
    std::vector<std::string> charNames = {"Monkey", "Monk", "Piggy", "Sandy"};
    int characterHeight = vp.metersToPixels(1.7);
    int characterWidth = vp.metersToPixels(1.7);
    int charPresentCount = 0;
    for (int i = 0; i < 4; i++) {
        if (characterStruct[charNames[i]]["book"]["boolList"]["Present"]) {
            double startX = vp.metersToPixels((double)characterStruct[charNames[i]]["centerXM"]);
            double startY = vp.metersToPixels((double)characterStruct[charNames[i]]["centerYM"]);

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
            
            double headScale = .198 * screenScale;

            head->setScale(headScale);
            head->setPosition(UI_HEAD_START + (charPresentCount * cocos2d::Vec2(UI_HEAD_INC, 0)));
 
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
            auto label = cocos2d::Label::createWithTTF(buttonString, "fonts/WaitingfortheSunrise.ttf", 40 * screenScale);
            label->setTextColor(cocos2d::Color4B::WHITE);
            label->enableOutline(cocos2d::Color4B::BLACK, 1);
            label->enableShadow();
            label->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
            label->setPosition(UI_HEAD_START + (charPresentCount * cocos2d::Vec2(UI_HEAD_INC, 0)) + cocos2d::Vec2(0, UI_HEAD_INC));
            uiLayer->addChild(label, 10);
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
    player->_controlledCharacter->toggleToPlayer();

    nlohmann::json boulderJson = lvl["boulders"];
    for (auto& bAtt: boulderJson) {
        cocos2d::Vec2 center = vp.metersToPixels(cocos2d::Vec2((double)bAtt["centerXM"], (double)bAtt["centerYM"]));
        std::stringstream ss;
        std::string temp = bAtt["path"];
        ss << "assets/" << temp;
        std::string imageName = ss.str();
        double mass = bAtt["book"]["doubList"]["Mass"];
        bool type = bAtt["book"]["boolList"]["Polygon collision"];
        cocos2d::Size imgSize = cocos2d::Size(vp.metersToPixels((double)bAtt["scaledIGWM"]), vp.metersToPixels((double)bAtt["scaledIGHM"]));
        Boulder *b;
        if (!type) {
            b = new Boulder(
                    vp.metersToPixels((double)bAtt["book"]["doubList"]["Radius"]),
                    imageName,
                    center,
                    mass,
                    imgSize);
        } else { // if (type == "POLYGON") {
            std::vector<cocos2d::Vec2> points;
            for (auto& point: bAtt["book"]["collPointList"]) {
                points.push_back(vp.metersToPixels(cocos2d::Vec2((double)point["x"], (double)point["y"])));
            }
            b = new Boulder(points, imageName, center, mass, imgSize);
        }
        /*else {
            std::cout << "Boulder type should be either CIRCLE or POLYGON, not " << type << std::endl;
            throw std::invalid_argument("Boulder type should be circle or polygon");
        } */
        if (bAtt["book"]["boolList"]["StartFixed"]) {
            b->getBody()->setDynamic(false);
        }
        boulders[(int)bAtt["ticket"]] = b;
        levelLayer->addChild(b, BOULDER_Z);
    }
    
    nlohmann::json inputpegs = lvl["pegs"];
    for (auto& gAtt: inputpegs) {
        if (gAtt.is_null() || gAtt["scaledIGWM"].is_null()) {
            std::cout << "WTF IS HAPPENING?" << std::endl;
            continue;
        }
        if (!gAtt["path"].is_string()) {
            throw std::domain_error("Golden peg doesn't have an image name!");
        }
        std::string imageName = gAtt["path"];
        imageName = "assets/" + imageName;
        cocos2d::Size imageSize = cocos2d::Size(vp.metersToPixels((double)gAtt["scaledIGWM"]), vp.metersToPixels((double)gAtt["scaledIGHM"]));
        cocos2d::Vec2 center = vp.metersToPixels(cocos2d::Vec2((double)gAtt["centerXM"], (double)gAtt["centerYM"]));
        double rotation = -(double)gAtt["book"]["doubList"]["Rotation (deg)"];
        std::vector<Boulder *> bouldersToRelease = std::vector<Boulder *>();
        for (auto i: gAtt["book"]["intList"]) {
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
        double width = vp.metersToPixels((double)vAtt["book"]["doubList"]["Width"]);
        double length = vp.metersToPixels((double)vAtt["book"]["doubList"]["Length"]);
        double startingAngVel = vAtt["book"]["doubList"]["Velocity"];
        cocos2d::Vec2 center = cocos2d::Vec2(vp.metersToPixels((double)vAtt["centerXM"]),
                  vp.metersToPixels((double)vAtt["centerYM"]));
        std::string imageName = vAtt["path"];
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
        auto j = cocos2d::PhysicsJointPin::construct(v->getBody(), b, cocos2d::Vec2(0, ideal2Res(length/2.0)), cocos2d::Vec2::ZERO);
        _w->addJoint(j);
        levelLayer->addChild(n, VINE_Z);
    }
    
    nlohmann::json in_traps = lvl["traps"];
    for (auto& tAtt: in_traps) {
        // TODO: Read in density and inner and outer box.
        double wallWidth = vp.metersToPixels((double)tAtt["book"]["doubList"]["wallThickness"]);
        double trapWidth = vp.metersToPixels((double)tAtt["book"]["doubList"]["trapWidth"]);
        double trapHeight = vp.metersToPixels((double)tAtt["book"]["doubList"]["trapHeight"]);
        double offset = vp.metersToPixels((double)tAtt["book"]["doubList"]["offset"]);
        cocos2d::Size imgSize(vp.metersToPixels((double)tAtt["scaledIGWM"]), vp.metersToPixels((double)tAtt["scaledIGHM"]));
        cocos2d::Vec2 center = vp.metersToPixels(cocos2d::Vec2((double)tAtt["centerXM"], (double)tAtt["centerYM"]));
        
        std::string imageName = tAtt["path"];
        cocos2d::PhysicsMaterial material = cocos2d::PhysicsMaterial(tAtt["book"]["doubList"]["density"], tAtt["book"]["doubList"]["bounciness"], tAtt["book"]["doubList"]["friction"]);
        imageName = "assets/" + imageName;
        CageTrap *rS = new CageTrap(imageName, center, material, cocos2d::Size(trapWidth, trapHeight), imgSize, wallWidth, offset);
        trapsToTrigger.push_back(rS);
        levelLayer->addChild(rS, 10);
    }
    
    if (lvl["attackZones"].is_object()) {
        nlohmann::json zones = lvl["attackZones"];
        for (auto& zAtt : zones) {
            std::cout << "Reading zone" << std::endl;
            attackZones.push_back(Zone(
                    vp.metersToPixels(cocos2d::Vec2((double)zAtt["book"]["doubList"]["xmin"], (double)zAtt["book"]["doubList"]["ymin"])),
                    vp.metersToPixels(cocos2d::Vec2((double)zAtt["book"]["doubList"]["xmax"], (double)zAtt["book"]["doubList"]["ymax"])),
                    createFactoryFromJson(zAtt, vp)));
        }
    } else {
        std::cout << "Note: no zones in this level." << std::endl;
    }
    
    if (lvl["textTips"].is_object()) {
        nlohmann::json in_tutorials = lvl["textTips"];
        for (auto &tAtt : in_tutorials) {
            std::string tipString = tAtt["book"]["strList"]["text"];
            int fontSize = tAtt["book"]["intList"]["fontSize"];
            cocos2d::Vec2 center = vp.metersToPixels(cocos2d::Vec2((double)tAtt["centerXM"], (double)tAtt["centerYM"]));
            auto label = cocos2d::Label::createWithTTF(tipString, "fonts/WaitingfortheSunrise.ttf", fontSize);
            label->setTextColor(cocos2d::Color4B::WHITE);
            label->enableOutline(cocos2d::Color4B::BLACK, 1);
            label->enableShadow();
            label->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
            label->setPosition(center);
            levelLayer->addChild(label, 10);
        }
    }

    levelEnd.x = vp.metersToPixels((double)lvl["eolPoint"]["x"]);
    levelEnd.y = vp.metersToPixels((double)lvl["eolPoint"]["y"]);
    if (lvl["eolQuadrant"].is_number_integer()) {
        levelEndDir = lvl["eolQuadrant"];
    } else {
        levelEndDir = 4;
    }
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
    // 1.7/130.0 means that 1.7 meters in the game world (average human male height) is represented by 130 pixels on screen.
    vp = Viewpoint(visibleSize, 1.7/130.0);

    //try {
        std::ifstream inFile(levelToLoad);
        nlohmann::json lvl;
        inFile >> lvl;
        if (lvl["VERSION"].is_null()) {
            std::cout << "We no longer support this old file format." << std::endl;
        } else if (lvl["VERSION"].is_number() && (int)lvl["VERSION"] == 2) {
            layer = parseLevelFromJsonV2(lvl, debugOn);
        }
    /*}
    catch (std::domain_error ex) {
        std::cout<< "Json was mal-formed, or expected members were not found, " << ex.what() << std::endl;
       return false;
    } catch (std::invalid_argument ex) {
        std::cout<< "Json was mal-formed, or expected members were not found, " << ex.what() << std::endl;
        return false;
    } */
 
    if (layer == nullptr) {
        std::cout << "Level file corrupted!" << std::endl;
        return false;
    }
    
    // Deal with scale stuff.
    layer->setScale(screenScale);
    uiLayer->setScale(screenScale);
    
    vp.setLayer(layer);
    vp.setScale(screenScale);

    this->addChild(layer, LVL_LAYER_Z_IDX);
    
    eventListener = cocos2d::EventListenerKeyboard::create();
    eventListener->retain();
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
                if (!m->isVisible()) {
                    this->pauseScene();
                }
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
                // Set the cinematic look.
                auto sprite = cocos2d::Sprite::create("assets/BlackBar.png");
                auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
                auto origin = cocos2d::Director::getInstance()->getVisibleOrigin();
                sprite->setContentSize(cocos2d::Size(visibleSize.width, visibleSize.height / 12.0));
                sprite->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
                sprite->setPosition(cocos2d::Vec2(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (25.0/24.0)));
                
                auto sprite2 = cocos2d::Sprite::create("assets/BlackBar.png");
                sprite2->setContentSize(cocos2d::Size(visibleSize.width, visibleSize.height / 12.0));
                sprite2->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
                sprite2->setPosition(cocos2d::Vec2(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (-1.0/24.0)));
                
                auto upMoveDown = cocos2d::MoveTo::create(2.0, cocos2d::Vec2(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (23.0/24.0)));
                auto downMoveUp = cocos2d::MoveTo::create(2.0, cocos2d::Vec2(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (1.0/24.0)));
                
                uiLayer->addChild(sprite, 10);
                uiLayer->addChild(sprite2, 10);
                
                sprite->runAction(upMoveDown);
                sprite2->runAction(downMoveUp);
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
    contactListener->onContactPostSolve = CC_CALLBACK_1(MainGameScene::onContactPostSolve, this);
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    
    //// Setup pause menu.
    auto continueNode = createSunriseLabel("Continue", 100, JTTW::screenScale);
    auto exitNode = createSunriseLabel("Exit to Main Menu", 100, JTTW::screenScale);
    
    cocos2d::MenuItemLabel *playItem = cocos2d::MenuItemLabel::create(continueNode, CC_CALLBACK_0(MainGameScene::resumeScene, this));
    cocos2d::MenuItemLabel *exitItem = cocos2d::MenuItemLabel::create(exitNode, CC_CALLBACK_0(MainGameScene::menuCloseCallback, this));
    
    options.push_back(playItem);
    options.push_back(exitItem);
    currentOption = options.begin();
    (*currentOption)->setColor(SELECTED);
    (*currentOption)->selected();

    double middleX = origin.x + visibleSize.width / 2.0;
    playItem->setPosition(middleX, origin.y + visibleSize.height * (4.0/ 9.0));
    exitItem->setPosition(middleX, origin.y + visibleSize.height * (2.0/ 9.0));
    
    m = cocos2d::Menu::create(playItem, exitItem, NULL);
    m->setPosition(0,0);

    m->setVisible(false);

    this->addChild(m, 10);
    
    pauseListener = cocos2d::EventListenerKeyboard::create();
    pauseListener->retain();
    pauseListener->onKeyPressed = [this](cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
        switch(keyCode) {
            case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
                (*currentOption)->setColor(UNSELECTED);
                (*currentOption)->unselected();
                if (currentOption == options.begin()) {
                    currentOption = options.end();
                }
                currentOption--;
                (*currentOption)->setColor(SELECTED);
                (*currentOption)->selected();
                break;
            
            case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
                (*currentOption)->setColor(UNSELECTED);
                (*currentOption)->unselected();
                currentOption++;
                if (currentOption == options.end()) {
                    currentOption = options.begin();
                }
                (*currentOption)->setColor(SELECTED);
                (*currentOption)->selected();
                break;
            
            case cocos2d::EventKeyboard::KeyCode::KEY_ENTER:
                (*currentOption)->activate();
                break;
                
            default:
                // do nothing.
                break;
        }

    };
    
    curtain = cocos2d::Sprite::create("assets/BlackBar.png");
    curtain->setContentSize(cocos2d::Size(visibleSize.width, visibleSize.height));
    curtain->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    curtain->setPosition(origin);
    curtain->setCascadeOpacityEnabled(true);
    curtain->setOpacity(170);
    curtain->setVisible(false);
    uiLayer->addChild(curtain, 10);

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

void MainGameScene::menuCloseCallback() {
    audio->stopBackgroundMusic();
    audio->preloadBackgroundMusic("Music/MenuScreen.mp3");
    audio->playBackgroundMusic("Music/MenuScreen.mp3");
    auto startScene = MainMenu::createScene();
    this->_eventDispatcher->removeEventListener(pauseListener);
    this->_eventDispatcher->removeEventListener(eventListener);
    eventListener->release();
    pauseListener->release();
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
        if (characters[i]->_respawnNextCycle) {
            // TODO: have characters fall down when hit.
            characters[i]->restartFromRespawn();
            done = false;
            characters[i]->_respawnNextCycle = false;
            continue;
        }
        for (auto& zone : attackZones) {
            if (zone.containsPoint(characters[i]->getPosition())) {
                if (attacking.find(characters[i]) == attacking.end()) {
                    std::cout << "Attacking " << characters[i]->characterName << "!" << std::endl;
                    attacking[characters[i]] = zone.getFactory();
                    attackCountdown[characters[i]] = 1.0;
                }
            } else {
                attacking.erase(characters[i]);
            }
        }
        for (auto &spawn : respawnPoints) {
            if (characters[i]->getPosition().x > spawn.x && spawn.x > characters[i]->getRespawnProgress()) {
                characters[i]->setNewRespawn(spawn);
            } 
        }
        if (characters[i]->getPosition().y < ideal2Res(-100)) { // TODO: un-hardcode this.
            done = false; // don't go to the next level if all characters die at once!
            characters[i]->restartFromRespawn();
        }  else {
            // Quadrant stuff.
            cocos2d::Vec2 pos = characters[i]->getPosition();
            if ((levelEndDir == 1 && (pos.x < levelEnd.x || pos.y < levelEnd.y)) ||
                (levelEndDir == 2 && (pos.x > levelEnd.x || pos.y < levelEnd.y)) ||
                (levelEndDir == 3 && (pos.x > levelEnd.x || pos.y > levelEnd.y)) ||
                (levelEndDir == 4 && (pos.x < levelEnd.x || pos.y > levelEnd.y))) {
                done = false;
            }
        }
    }
    if (done) {
        for (auto &c: characters) {
            c->stop();
        }
        nextLevelCallback();
    }
    
    for (auto &m : moveables) {
        m->move(delta);
    }
    
    for (auto entry = attacking.begin(); entry != attacking.end(); entry++) {
        if (attackCountdown[entry->first] <= 0) {
            attackCountdown[entry->first] = 5.0;
            auto sprite = entry->second->generateProjectile(entry->first->getPosition());
            layer->addChild(sprite);
            break;
        }
        attackCountdown[entry->first] = attackCountdown[entry->first] - delta;
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


////////// Pause menu stuff. ///////////////////
void MainGameScene::pauseScene() {
    for (auto &c : characters) {
        c->setTimeScale(0.0);
    }
    curtain->setVisible(true);
    m->setVisible(true);
    Director::getInstance()->getRunningScene()->getPhysicsWorld()->setSpeed(0);
    //this->_eventDispatcher->removeEventListener(eventListener);
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(pauseListener, this);
}


void MainGameScene::resumeScene() {
    for (auto &c : characters) {
        c->setTimeScale(1.0);
    }
    curtain->setVisible(false);
    m->setVisible(false);
    Director::getInstance()->getRunningScene()->getPhysicsWorld()->setSpeed(1);
    this->_eventDispatcher->removeEventListener(pauseListener);
    //this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
}


