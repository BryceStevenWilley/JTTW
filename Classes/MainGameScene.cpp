#include "MainGameScene.h"
#include "SimpleAudioEngine.h"
#include "MainMenuScene.hpp"
#include "DisappearingPlatform.hpp"
#include "Collisions.hpp"
#include "Monkey.hpp"
#include "Piggy.hpp"
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
#include <chrono>
#include "Cutscene.hpp"
#include "ZoneCutscene.hpp"
#include "EndLevelCutscene.hpp"
#include "SquishObject.hpp"
#include "SinkObject.hpp"
#include "Quicksand.hpp"

using namespace cocos2d;
using namespace JTTW;

const int UI_LAYER_Z_IDX = 2;
const int LVL_LAYER_Z_IDX = 1;

const double MASS_TO_SINK_CLOUD = ideal2Res(ideal2Res(8500));

const int GRAVITY = ideal2Res(-750);
const cocos2d::Vec2 UI_HEAD_START = cocos2d::Vec2(ideal2Res(40.0), ideal2Res(40.0));
const double UI_HEAD_INC = 70;

cocos2d::Scene* MainGameScene::createScene(std::string levelToLoad) {
    // 'scene' and layer are autorelease objects.
    auto scene = cocos2d::Scene::createWithPhysics();
    scene->getPhysicsWorld()->setGravity(cocos2d::Vec2(0, GRAVITY));
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
             cocos2d::Vec2 collisionPosition = m->getPosition();
             float distFromCenter = ideal2Res(collisionPosition.distance(v->getPosition()));
             float distFromRotCenter = ideal2Res(collisionPosition.distance(v->getRotationCenter()));
             float halfLength = ideal2Res(v->getRotationCenter().distance(v->getPosition()));
             float offset;
             if (distFromRotCenter > halfLength && distFromCenter < halfLength) {
                 std::cout << "Entering vine at the bottom of the vine." << std::endl;
                 offset = -distFromCenter;
             } else if (distFromRotCenter < halfLength && distFromCenter < halfLength) {
                 std::cout << "Entering vine from the top of the vine." << std::endl;
                 offset = distFromCenter;
             } else {
                 // IDK what's happening?
                 std::cout << "distFromCenter" << distFromCenter << ", from rot center: " << distFromRotCenter << ", half length: " << halfLength << std::endl;
                 std::cout << "Too far below or above, offset is now " << -(halfLength - ideal2Res(5)) << std::endl;
                 offset = -(halfLength - ideal2Res(5));
             }
             m->enteringVine(this->getScene()->getPhysicsWorld(), v, offset, false);
             return true;
        } else {
            //m->leavingClimeable();
            return true; // don't rebalance impulse!
        }
    }
    if (node->getTag() == PROJECTILE_TAG || node->getTag() == INSTANT_DEATH_TAG) {
        if (begin) {
            std::cout << "Instant death for " << c->characterName << std::endl;
            c->die(Character::CauseOfDeath::PROJECTILE);
            if (node->getTag() == PROJECTILE_TAG) {
                node->removeFromParent();
                deleteTimer.erase(deleteTimer.find((cocos2d::Sprite *)node));
            }
            return true;
        }
    }
    if (node->getTag() == PEG_TAG && c->characterName == "Monk") {
        if (begin) {
            // Add peg to Monk's peg hitting thing.
            Peg *peg = (Peg *)node;
            peg->setToTrigger();
            return false;
        }
    }

    if (node->getTag() == QUICKSAND_TAG) {
        if (begin) {
            c->landedInQuicksand((Quicksand *)node);
        } else {
            c->leftQuicksand();
        }
        return false;
    }

    if (node->getTag() == CHARACTER_TAG) {
        // Characters colliding: anyone and Monkey?
        Character *c2 = (Character *)node;
        if ((c->characterName == "Monkey" || c2->characterName == "Monkey") && begin) {
            Monkey *m;
            Character *other;
            if (c->characterName == "Monkey") {
                m = (Monkey *) c;
                other = c2;
            }
            if (c2->characterName == "Monkey") {
                m = (Monkey *) c2;
                other = c;
            }
            if (m->getMonkeyState() == Monkey::SWINGING && !m->hasHangingCharacter()) {
                m->setHangingCharacter(other);
                auto vec = other->getPosition() - m->getPosition();
                
                other->enteringHanging(this->getScene()->getPhysicsWorld(), m, cocos2d::Vec2(ideal2Res(vec.x), ideal2Res(vec.y)), false);
                return true;
            }
        }
        if (c->characterName == "Piggy" || c2->characterName == "Piggy") {
            Piggy *p;
            Character *other;
            if (c->characterName == "Piggy") {
                p = (Piggy *) c;
                other = c2;
            }
            if (c2->characterName == "Piggy") {
                p = (Piggy *) c2;
                other = c;
            }
            if (begin) {
                p->addHeldCharacter(other);
            } else {
                p->removeHeldCharacter(other);
            }
            return true;
        }
        return false;
    }
    if (normal.dot(cocos2d::Vec2(0, -1)) > std::cos(M_PI * (1.2/ 4.0))) {
        if (begin) {
            c->landedCallback(body, normal.getPerp());
            
            if (node->getTag() == SINKABLE_TAG) {
                SinkObject *sink = (SinkObject *) node;
                std::cout << c->characterName << "'s mass is " << c->getMass() << std::endl;
                std::cout << "Needs to be " << MASS_TO_SINK_CLOUD << std::endl;
                if (c->getMass() > MASS_TO_SINK_CLOUD) {
                    sink->landedCallback();
                }
            }
            return true;
        } else { // onContactEnd
            c->leftCallback(body);
            if (node->getTag() == SINKABLE_TAG && c->getMass() > MASS_TO_SINK_CLOUD) {
                ((SinkObject *)node)->removeCallback();
            }
            return true;
        }
    } else if (node->getTag() == CLIMBEABLE_TAG && c->characterName == "Monkey") {
        // TODO: hacky, fix
        Monkey *m = (Monkey *)c;
        Platform *p = (Platform *)node;
        if (begin) {
            std::cout << "Monkey hit tree" << std::endl;
            cocos2d::Vec2 collisionPosition = m->getPosition();
            m->enteringClimeable(this->getScene()->getPhysicsWorld(), p, (collisionPosition - p->getPosition()) / 1.7, normal.getPerp(), false);
        } else {
            std::cout << "Monkey is jumping off of the tree!" << std::endl;
            m->leavingClimeable(false, false);
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
    if (nodeA == NULL || nodeB == NULL) {
        return false;
    }
    
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

bool characterPostSolve(cocos2d::PhysicsContact& contact, Character *c, cocos2d::PhysicsBody *body, Node *node, cocos2d::Vec2 normal) {
    return true;
}

void MainGameScene::onContactPostSolve(cocos2d::PhysicsContact& contact, const cocos2d::PhysicsContactPostSolve &solve) {
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    auto normal = contact.getContactData()->normal;
    
    if (nodeA->getTag() == CHARACTER_TAG) {
        //Character *c = (Character *)nodeA;
        //characterPostSolve(contact, c, contact.getShapeB()->getBody(), nodeB, normal);
    }
    
    if (nodeB->getTag() == CHARACTER_TAG) {
        //Character *c = (Character *)nodeB;
        //characterPostSolve(contact, c, contact.getShapeA()->getBody(), nodeA, -normal);
    }
}

const int PLATFORM_Z = 4;
const int VINE_Z = 3;
const int BOULDER_Z = 5;
const int CHARACTER_Z = 6;

cocos2d::Layer *MainGameScene::parseLevelFromJsonV2(nlohmann::json lvl, bool debugOn) {
    cocos2d::Layer *levelLayer = cocos2d::Layer::create();
    
    // Read in vp limits early.
    double xmin, xmax, ymin, ymax;
    if (lvl["viewportZone"].is_object()) {
        nlohmann::json doubs = lvl["viewportZone"]["book"]["doubList"];
        xmin = doubs["xmin"];
        xmax = doubs["xmax"];
        ymin = doubs["ymin"];
        ymax = doubs["ymax"];
        xmin = vp.metersToPixels(xmin);
        xmax = vp.metersToPixels(xmax);
        ymin = vp.metersToPixels(ymin);
        ymax = vp.metersToPixels(ymax);
    } else {
        // Just set the limits to be Huge.
        xmin = -1 * std::numeric_limits<double>::infinity();
        xmax = std::numeric_limits<double>::infinity();
        ymin = -1 * std::numeric_limits<double>::infinity();
        ymax = std::numeric_limits<double>::infinity();
    }
    
    vp.setLimits(cocos2d::Vec2(xmin, ymin), cocos2d::Vec2(xmax, ymax));
    
    // draw and add background
    nlohmann::json backgroundAtts = lvl["bg"];
    std::string bgPath = backgroundAtts["path"];
    bgPath = "assets/" + bgPath;
    background = cocos2d::Sprite::create(bgPath);
    background->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    if (xmax != std::numeric_limits<double>::infinity()) {
        background->setContentSize(cocos2d::Size((xmax - xmin) + 200, (ymax - ymin) + 200));
        background->setPosition(xmin, ymin);
        levelLayer->addChild(background, -8);
    } else {
        background->setScale(1.4);
        background->setPosition(0,-300.0);
        this->addChild(background, -8);
    }

    vp.background = background;
    
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
        
        std::vector<double> frictions;
        for (auto& f : pAtt["book"]["edgeFrictionList"]) {
            frictions.push_back(f);
        }
        
        std::vector<bool> deathEdges;
        for (auto &d : pAtt["book"]["deadlyEdgeList"]) {
            deathEdges.push_back(d);
        }
        
        std::string typeText;
        
        if (pAtt["book"]["boolList"]["Moving"].is_boolean()) {
            if (pAtt["book"]["boolList"]["Moving"]) {
                typeText = "MOVING";
            } else {
                typeText = "STATIC";
            }
        }
        
        if(pAtt["book"]["strList"]["interactable.Platform$Type"].is_string()) {
            typeText = pAtt["book"]["strList"]["interactable.Platform$Type"];
        }
        
        if (pAtt["book"]["boolList"]["Sinkable"].is_boolean() && pAtt["book"]["boolList"]["Sinkable"]) {
            std::cout <<" Sinkable!" << std::endl;
            cocos2d::Vec2 centerA(centerX, centerY);
            double centerBX = vp.metersToPixels((double)pAtt["endpoint"]["x"]);
            double centerBY = vp.metersToPixels((double)pAtt["endpoint"]["y"]);
            cocos2d::Vec2 centerB(centerBX, centerBY);
            double maximumVelocity = ideal2Res(vp.metersToPixels((double)pAtt["book"]["doubList"]["Velocity"]));
            SinkObject *p = new SinkObject(fullImagePath, centerB, centerA, cocos2d::Size(imageSizeWidth, imageSizeHeight), ps, frictions, deathEdges, maximumVelocity);

            levelLayer->addChild(p, z);
            platforms.push_back(p);
            moveables.push_back(p);
            continue;
        }
        
        if (typeText == "MOVING") {
            cocos2d::Vec2 centerA(centerX, centerY);
            double centerBX = vp.metersToPixels((double)pAtt["endpoint"]["x"]);
            double centerBY = vp.metersToPixels((double)pAtt["endpoint"]["y"]);
            cocos2d::Vec2 centerB(centerBX, centerBY);
            double maximumVelocity = ideal2Res(vp.metersToPixels((double)pAtt["book"]["doubList"]["Velocity"]));
            MoveablePlatform *p = new MoveablePlatform(fullImagePath, centerA, centerB, cocos2d::Size(imageSizeWidth, imageSizeHeight), ps, frictions, maximumVelocity);

            std::cout << fullImagePath << std::endl;
            levelLayer->addChild(p, z);
            platforms.push_back(p);
            moveables.push_back(p);
        } else if (typeText == "STATIC") {
            if (pAtt["book"]["boolList"]["Disappears"]) {
                DisappearingPlatform *p = new DisappearingPlatform(fullImagePath, cocos2d::Vec2(centerX, centerY), cocos2d::Size(imageSizeWidth, imageSizeHeight),
                   ps, pAtt["book"]["boolList"]["Climbable"], pAtt["book"]["boolList"]["Collidable"]);
                levelLayer->addChild(p, z);
                trapsToTrigger.push_back(p);
            } else {
                Platform *p = new Platform(fullImagePath, cocos2d::Vec2(centerX, centerY), cocos2d::Size(imageSizeWidth, imageSizeHeight), ps, frictions, deathEdges, pAtt["book"]["boolList"]["Climbable"], pAtt["book"]["boolList"]["Collidable"]);
                levelLayer->addChild(p, z);
                if (p->getDeathBody() != nullptr) {
                    std::cout << "Adding a death body" << std::endl;
                    levelLayer->addChild(p->getDeathBody(), z);
                }
                platforms.push_back(p);
            }
        } else { // typeText == "HITTING"
            cocos2d::Vec2 centerA(centerX, centerY);
            double centerBX = vp.metersToPixels((double)pAtt["endpoint"]["x"]);
            double centerBY = vp.metersToPixels((double)pAtt["endpoint"]["y"]);
            cocos2d::Vec2 centerB(centerBX, centerBY);
            double velocity = ideal2Res(vp.metersToPixels((double)pAtt["book"]["doubList"]["Velocity"]));
            double quickVelocity = ideal2Res(vp.metersToPixels((double)pAtt["book"]["doubList"]["Quick Velocity"]));
            double pauseTime = pAtt["book"]["doubList"]["Pause time"];
            SquishObject *q = new SquishObject(fullImagePath, centerB, centerA, cocos2d::Size(imageSizeWidth, imageSizeHeight), ps, frictions, deathEdges, velocity, quickVelocity, pauseTime);
            levelLayer->addChild(q, z);
            if (q->getDeathBody() != nullptr) {
                std::cout << "Adding a death body" << std::endl;
                levelLayer->addChild(q->getDeathBody(), z);
            }
            moveables.push_back(q);
        }
    }
    nlohmann::json characterStruct = lvl["characters"];
    std::vector<std::string> charNames = {"Monkey", "Monk", "Piggy", "Sandy"};
    int characterHeight = vp.metersToPixels(1.7);
    int characterWidth = vp.metersToPixels(1.7);
    int charPresentCount = 0;
    int totalPresentCount = 0;
    for (int i = 0; i < 4; i++) {
        if (characterStruct[charNames[i]]["book"]["boolList"]["Present"]) {
            totalPresentCount++;
        }
    }
    
    for (int i = 0; i < 4; i++) {
        if (characterStruct[charNames[i]]["book"]["boolList"]["Present"]) {
            auto strList = characterStruct[charNames[i]]["book"]["strList"];
            if (strList["after"].is_string() && strList["after"] != "") {
                
            }
            double startX = vp.metersToPixels((double)characterStruct[charNames[i]]["centerXM"]);
            double startY = vp.metersToPixels((double)characterStruct[charNames[i]]["centerYM"]);

            Character *c = Character::createFromName(charNames[i], cocos2d::Vec2(startX, startY), cocos2d::Size(characterWidth, characterHeight));
            characters.push_back(c);
            levelLayer->addChild(c, CHARACTER_Z);
            AiAgent *agent = new AiAgent(c);
            agent->setPlayerPosOffset(c->getPosition() - characters[0]->getPosition());
            agents.push_back(agent);
            
            if (totalPresentCount > 1) {
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
                
                auto heyLookOverHere = cocos2d::Sequence::create(
                        cocos2d::ScaleBy::create(2.5, 1.0), // Wait for 2 second.
                        cocos2d::ScaleBy::create(.7, 2.7),
                        cocos2d::ScaleBy::create(.6, 1.0), // Wait for another second.
                        cocos2d::ScaleBy::create(.7, 1.0/2.7),
                        NULL);
                head->runAction(heyLookOverHere);
                charPresentCount++;
            }
        }
    }
    
    if (characterStruct["Dragon"].is_object()) {
        if (characterStruct["Dragon"]["book"]["boolList"]["Present"]) {
            // Add the dragon to the level.
            double startX = vp.metersToPixels((double)characterStruct["Dragon"]["centerXM"]);
            double startY = vp.metersToPixels((double)characterStruct["Dragon"]["centerYM"]);
            dragon = new Dragon(cocos2d::Vec2(startX, startY), cocos2d::Size(vp.metersToPixels(16), vp.metersToPixels(8)));
            levelLayer->addChild(dragon, CHARACTER_Z - 5);
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
    
    if (inputpegs.is_object() && inputpegs.size() != 0) {
        Monkey *m = (Monkey *)characters[0];
        m->setBoulderBury();
    }
    
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
        if (((double)vAtt["book"]["doubList"]["Arc Length (deg)"]) == 0.0) {
            v->getBody()->setRotationEnable(false);
        }
        
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
            int z;
            if (tAtt["book"]["intList"]["Z-order"].is_number_integer()) {
                z = tAtt["book"]["intList"]["Z-order"];
            } else {
                z = 10;
            }
            cocos2d::Vec2 center = vp.metersToPixels(cocos2d::Vec2((double)tAtt["centerXM"], (double)tAtt["centerYM"]));
            auto label = cocos2d::Label::createWithTTF(tipString, "fonts/WaitingfortheSunrise.ttf", fontSize);
            label->setTextColor(cocos2d::Color4B::WHITE);
            label->enableOutline(cocos2d::Color4B::BLACK, 1);
            label->enableShadow();
            label->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
            label->setPosition(center);
            levelLayer->addChild(label, z);
        }
    }
    
    if (lvl["quicksand"].is_object()) {
        nlohmann::json in_quicksand = lvl["quicksand"];
        for (auto &qAtt: in_quicksand) {
            std::string fullImagePath = qAtt["path"];
            fullImagePath = "assets/" + fullImagePath;
            double centerX = vp.metersToPixels((double)qAtt["centerXM"]);
            double centerY = vp.metersToPixels((double)qAtt["centerYM"]);
            double imageSizeWidth = vp.metersToPixels((double)qAtt["scaledIGWM"]);
            double imageSizeHeight = vp.metersToPixels((double)qAtt["scaledIGHM"]);
        
            int z;
            if (qAtt["book"]["doubList"]["Z-order"].is_number()) {
                z = (int)qAtt["book"]["doubList"]["Z-order"];
            } else {
                std::cout << "Using default z-order for " << fullImagePath << qAtt["ticket"] << std::endl;
                z = PLATFORM_Z;
            }
        
            std::vector<cocos2d::Vec2> ps;
            for (auto& cPoint: qAtt["book"]["collPointList"]) {
                ps.push_back(cocos2d::Vec2(vp.metersToPixels((double)cPoint["x"]), vp.metersToPixels((double)cPoint["y"])));
            }
        
            double sinkVel = vp.metersToPixels((double)qAtt["book"]["doubList"]["sinkVel"]);
            double recoverVel = vp.metersToPixels((double)qAtt["book"]["doubList"]["recoveryVel"]);

            Quicksand *qq = new Quicksand(fullImagePath, cocos2d::Vec2(centerX, centerY), cocos2d::Size(imageSizeWidth, imageSizeHeight), ps, sinkVel, recoverVel);
            
            levelLayer->addChild(qq, z);
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
        audio->stopBackgroundMusic();
        audio->playBackgroundMusic((std::string("Music/") + levelName + std::string(".mp3")).c_str(), true);
    }
    
    if(lvl["endQuote"].is_string()) {
        _endQuote = lvl["endQuote"];
    }
    
    if (lvl["levelFileName"] == "dragon") {
        Zone cutSceneZone = Zone(vp.metersToPixels(cocos2d::Vec2(25, 3)), vp.metersToPixels(cocos2d::Vec2(40, 20)), nullptr);
        cutscenes.push_back(new ZoneCutscene(Cutscene::Scene::DRAGON, vp, cutSceneZone));
    }
    
    if (lvl["levelFileName"] == "GardenEscape") {
        endScene = new EndLevelCutscene(Cutscene::Scene::FLIGHT, vp);
    }
    
    if (lvl["levelFileName"] == "bodhi") {
        endScene = new EndLevelCutscene(Cutscene::Scene::BODHI, vp);
    }
    
    return levelLayer;
}

bool MainGameScene::init(std::string levelToLoad, cocos2d::PhysicsWorld *w) {
    if ( !Layer::init() ) {
        return false;
    }
    
    _currentLevelName = levelToLoad;
    
    _w = w;
    audio = CocosDenshion::SimpleAudioEngine::getInstance();
    
    // aka window dimensions
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    std::cout << "VisibleSize: " << visibleSize.width << ", " << visibleSize.height << std::endl;
    std::cout << "Resolution Size: " << actualResolution.width << ", " << actualResolution.height << std::endl;
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();

    // Creates the camera, or viewpoint for this scene.
    // 1.7/130.0 means that 1.7 meters in the game world (average human male height) is represented by 130 pixels on screen.
    vp = Viewpoint(visibleSize, 1.7/130.0);

    uiLayer = cocos2d::Layer::create();
    uiLayer->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    
    //try {
        std::ifstream inFile(levelToLoad);
        nlohmann::json lvl;
        inFile >> lvl;
        if (lvl["VERSION"].is_null()) {
            std::cout << "We no longer support this old file format." << std::endl;
        } else if (lvl["VERSION"].is_number() && (int)lvl["VERSION"] == 2) {
            layer = parseLevelFromJsonV2(lvl, true);
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
    eventListener->onKeyPressed = CC_CALLBACK_2(MainGameScene::KeypressedCallback, this);
    eventListener->onKeyReleased = CC_CALLBACK_2(MainGameScene::KeyreleasedCallback, this);

    this->_eventDispatcher->addEventListenerWithFixedPriority(eventListener, 1);
    
    auto contactListener = cocos2d::EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(MainGameScene::onContactBegin, this);
    contactListener->onContactSeparate = CC_CALLBACK_1(MainGameScene::onContactEnd, this);
    contactListener->onContactPostSolve = CC_CALLBACK_2(MainGameScene::onContactPostSolve, this);
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    
    auto restartNode = createSunriseLabel("r = restart", 30, JTTW::screenScale);
    restartNode->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_RIGHT);
    restartNode->setPosition(origin.x + visibleSize.width * (19.0/20.0), origin.y + ideal2Res(20));
    this->addChild(restartNode, LVL_LAYER_Z_IDX + 2);
    
    ///////////////////// Setup pause menu. ///////////////////
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
    pauseListener->onKeyPressed = CC_CALLBACK_2(MainGameScene::PausedKeyCallback, this);
    pauseListener->onKeyReleased = CC_CALLBACK_2(MainGameScene::PausedKeyReleasedCallback, this);
    
    curtain = cocos2d::Sprite::create("assets/BlackBar.png");
    curtain->setContentSize(cocos2d::Size(visibleSize.width, visibleSize.height) * 1.0 / screenScale);
    curtain->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    curtain->setPosition(origin);
    curtain->setCascadeOpacityEnabled(true);
    curtain->setOpacity(170);
    curtain->setVisible(false);
    this->addChild(curtain, 9);

    ///////////////////////// CUT Scene stuff ////////////////
    // Set the cinematic look.
    upStart = cocos2d::Vec2(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (25.0/24.0)) * 1.0 / screenScale;
    downStart = cocos2d::Vec2(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (-1.0/24.0)) * 1.0 / screenScale;
    
    blackBarUp = cocos2d::Sprite::create("assets/BlackBar.png");
    blackBarUp->setContentSize(cocos2d::Size(visibleSize.width, visibleSize.height / 12.0) * 1.0 / screenScale);
    blackBarUp->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    blackBarUp->setPosition(upStart);
    blackBarUp->retain();
    
    blackBarDown = cocos2d::Sprite::create("assets/BlackBar.png");
    blackBarDown->setContentSize(cocos2d::Size(visibleSize.width, visibleSize.height / 12.0));
    blackBarDown->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
    blackBarDown->setPosition(downStart);
    blackBarDown->retain();
    
    for (auto &cutscene : cutscenes) {
        cutscene->initFromScene(this);
    }
    
    if (endScene != nullptr) {
        endScene->initFromScene(this);
    }
    
    upVisible = cocos2d::Vec2(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (23.0/24.0)) * 1.0 / screenScale;
    downVisible = cocos2d::Vec2(origin.x + visibleSize.width / 2.0, origin.y + visibleSize.height * (1.0/24.0)) * 1.0 / screenScale;

    this->addChild(blackBarUp, 10);
    this->addChild(blackBarDown, 10);

    this->scheduleUpdate();
    return true;
}

void MainGameScene::KeypressedCallback(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
    if (_keylogging) {
        auto now = std::chrono::system_clock::now();
        std::time_t ttp = std::chrono::system_clock::to_time_t(now);

        std::cout << "Key " << (int)keyCode << " was pressed at " << std::ctime(&ttp) << std::endl;
        std::cout << "Player position: " << player->_controlledCharacter->getPosition().x << ", " << player->_controlledCharacter->getPosition().y << std::endl;
    }
    if (firstTouch) {
        for(auto &c : characters) {
            c->updateMass();
        }
        firstTouch = false;
    }
    haveReleased[keyCode] = true;
    stillPressed[keyCode] = true;
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
        case EventKeyboard::KeyCode::KEY_D:
            player->plan(characters, keyCode, true);
            for (auto xAgent = agents.begin(); xAgent != agents.end(); xAgent++) {
                if ((*xAgent) != player) {
                    (*xAgent)->changeBehavior(player->_controlledCharacter, keyCode, true);
                }
            }
            break;
            
        case EventKeyboard::KeyCode::KEY_R: {
            auto startScene = MainGameScene::createScene(_currentLevelName);
            if (m->isVisible()) {
                this->_eventDispatcher->removeEventListener(pauseListener);
            } else {
                this->_eventDispatcher->removeEventListener(eventListener);
            }
            cocos2d::Director::getInstance()->replaceScene(startScene);
            break;
        }

        case EventKeyboard::KeyCode::KEY_9:
            _keylogging = !_keylogging;
            std::cout << "Key logging: " << _keylogging << std::endl;
            break;
            
        case EventKeyboard::KeyCode::KEY_8:
            seeEndCutsceneCallback();
            break;
            
        // TODO: turn this into a separate function.
        case EventKeyboard::KeyCode::KEY_0:
            blackBarUp->runAction(cocos2d::MoveTo::create(1.0, upVisible));
            blackBarDown->runAction(cocos2d::MoveTo::create(1.0, downVisible));
            uiLayer->setVisible(false);
            break;
        case EventKeyboard::KeyCode::KEY_1:
            blackBarUp->runAction(cocos2d::MoveTo::create(1.0, upStart));
            blackBarDown->runAction(cocos2d::MoveTo::create(1.0, downStart));
            uiLayer->setVisible(true);
            break;
          
        default:
            // do nothing.
            break;
    }
}

void MainGameScene::KeyreleasedCallback(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
    if (_keylogging) {
        auto now = std::chrono::system_clock::now();
        std::time_t ttp = std::chrono::system_clock::to_time_t(now);
        std::cout << "Key " << (int)keyCode << " was released at " << std::ctime(&ttp) << std::endl;
        std::cout << "Player position: " << player->_controlledCharacter->getPosition().x << ", " << player->_controlledCharacter->getPosition().y << std::endl;
    }
    if (haveReleased[keyCode]) {
        player->plan(characters, keyCode, false);
        for (auto xAgent = agents.begin(); xAgent != agents.end(); xAgent++) {
            if ((*xAgent) != player) {
                (*xAgent)->changeBehavior(player->_controlledCharacter, keyCode, false);
            }
        }
    } else {
        haveReleased[keyCode] = true;
    }
    stillPressed[keyCode] = false;
}

void MainGameScene::PausedKeyCallback(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
    haveReleased[keyCode] = true;
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
        case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
            (*currentOption)->activate();
            break;
            
        default:
            // do nothing.
            break;
    }
}

void MainGameScene::PausedKeyReleasedCallback(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event) {
    haveReleased[keyCode] = true;
}

void MainGameScene::switchToCharacter(int charIndex) {
    if (charIndex > (int)agents.size() - 1) {
        return;
    }
    auto nextPlayer = agents[charIndex];
    if (nextPlayer == player) {
        return; // don't do anything.
    }
    
    // Turn off ever.
    for (auto &xAgent : agents) {
        xAgent->equipNoResistenceBehavior(player->_controlledCharacter);
    }
    
    nextPlayer->switchUserToHereFrom(player);
    player = nextPlayer;
    vp.panToCharacter(player->_controlledCharacter);
}

void MainGameScene::menuCloseCallback() {
    audio->stopBackgroundMusic();
    audio->preloadBackgroundMusic("Music/MenuScreen.mp3");
    audio->playBackgroundMusic("Music/MenuScreen.mp3", true);
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
        auto end = LevelEnd::createScene(_nextLevel, _endQuote);
        auto fade = cocos2d::TransitionFade::create(1.5, end);
        if (endScene == nullptr) {
            auto fade = cocos2d::TransitionFade::create(4.0, end);
            cocos2d::Director::getInstance()->replaceScene(fade);
        } else {
            endScene->runScene(true, fade);
        }
        nextLevelStarting = true;
    }
}

void MainGameScene::seeEndCutsceneCallback() {
    if (!nextLevelStarting) {
        this->_eventDispatcher->removeEventListener(eventListener);
        audio->stopBackgroundMusic();
        auto menu = MainMenu::createScene();
        auto fade = cocos2d::TransitionFade::create(1.5, menu);
        if (endScene == nullptr) {
            cocos2d::Director::getInstance()->replaceScene(fade);
        } else {
            endScene->runScene(true, fade);
        }
        nextLevelStarting = true;
    }
}

void MainGameScene::update(float delta) {
    if (!m->isVisible()) {
        std::vector<cocos2d::Sprite *> timersToRemove;
        for (auto x = deleteTimer.begin(); x != deleteTimer.end(); x++) {
            deleteTimer[x->first] -= delta;
            if (deleteTimer[x->first] <= 0.0) {
                if (x->first->getParent() != NULL) {
                    x->first->removeFromParent();
                    timersToRemove.push_back(x->first);
                }
            }
        }
    
        for (auto &r: timersToRemove) {
            deleteTimer.erase(deleteTimer.find(r));
        }
    }

    if (!nextLevelStarting) {
        for (auto& xAgent : agents) {
            if (xAgent == player) {
                xAgent->_controlledCharacter->continueMotion();
            } else {
                xAgent->plan(player->_controlledCharacter, characters);
                xAgent->executeControl(delta);
            }
        }
    } else {
        for (auto& xAgent : agents) {
            xAgent->_controlledCharacter->continueMotion();
        }
    }
    
    bool done = true;
    for (int i = 0; i < (int)characters.size(); i++) {
        characters[i]->updateLoop(delta);
        for (auto& zone : attackZones) {
            if (zone.containsPoint(characters[i]->getPosition())) {
                if (attacking.find(characters[i]) == attacking.end() || std::find(attacking[characters[i]].begin(), attacking[characters[i]].end(), zone.getFactory()) == attacking[characters[i]].end()) {
                    std::cout << "Attacking " << characters[i]->characterName << "!" << std::endl;
                    FactoryAndTimer fAndT(zone.getFactory(), -1);
                    attacking[characters[i]].push_back(fAndT);
                }
            } else {
                auto x = std::find(attacking[characters[i]].begin(), attacking[characters[i]].end(), zone.getFactory());
                if (x != attacking[characters[i]].end()) {
                    attacking[characters[i]].erase(x);
                }
            }
        }
        for (auto &spawn : respawnPoints) {
            if (characters[i]->getPosition().x > spawn.x && spawn.x > characters[i]->getRespawnProgress()) {
                characters[i]->setNewRespawn(spawn);
            } 
        }
        if (characters[i]->getPosition().y < ideal2Res(-100)) { // TODO: un-hardcode this.
            done = false; // don't go to the next level if all characters die at once!
            characters[i]->die(Character::CauseOfDeath::FALL);
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
        nextLevelCallback();
    }
    
    for (auto &m : moveables) {
        m->move(delta);
    }
    
    if (!m->isVisible()) {
        // Game is not paused
        for (auto charUnderAttack = attacking.begin(); charUnderAttack != attacking.end(); charUnderAttack++) {
            auto activeZones = charUnderAttack->second;
            for (size_t i = 0; i < activeZones.size(); i++) {
                if (activeZones[i].countdown <= 0) {
                    std::cout << "Shooting projectile from " << i<< std::endl;
                    attacking[charUnderAttack->first][i].countdown = 5.0;
                    auto sprite = activeZones[i].factory->generateProjectile(charUnderAttack->first->getPosition());
                    layer->addChild(sprite, 8);
                    deleteTimer[sprite] = 4.5;
                    std::cout << "Wait another " << activeZones[i].countdown << std::endl;
                } else {
                    attacking[charUnderAttack->first][i].countdown -= delta;
                }
            }
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
                        character->freeze(3.0);
                        break;
                    
                    case FALL:
                        character->freeze(10.0);
                        character->setAnimation(0, "fall", false);
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
    
    std::vector<Cutscene *> removeThese;
    for (auto &cut : cutscenes) {
        if (cut->trigger()) {
            // remove the trap from the array.
            removeThese.push_back(cut);
        }
    }
    
    for (auto cuts = removeThese.begin(); cuts != removeThese.end(); cuts++) {
        cutscenes.erase(std::remove(cutscenes.begin(), cutscenes.end(), *cuts), cutscenes.end());
    }
    
    if (pegs.size() != 0) {
        bool allTriggered = true;
        for (auto &p: pegs) {
            if (!p->isTriggered()) {
                allTriggered = false;
            }
            if (p->isSetToTrigger()) {
                p->triggerPeg();
                continue;
            }
        }
        
        if (allTriggered) {
            std::cout << "All pegs triggered!" << std::endl;
            pegs.clear();
            
            Monkey *m = (Monkey *)characters[0];
            layer->removeChild(m);
            m->setBoulderUnbury();
            layer->addChild(m, CHARACTER_Z);
            layer->draw();
        }
    }

    if (!nextLevelStarting) {
        // Gives us camera control during cutscenes.
        vp.followCharacter(player->_controlledCharacter, delta);
    }
}

////////// Pause menu stuff. ///////////////////
void MainGameScene::pauseScene() {
    for (auto &c : characters) {
        c->setTimeScale(0.0);
    }
    curtain->setVisible(true);
    m->setVisible(true);
    Director::getInstance()->getRunningScene()->getPhysicsWorld()->setSpeed(0);
    this->_eventDispatcher->removeEventListener(eventListener);
    haveReleased.clear();
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(pauseListener, this);
}

void MainGameScene::resumeScene() {
    for (auto &c : characters) {
        c->setTimeScale(1.0);
    }
    for (auto &r: haveReleased) {
        if (stillPressed[r.first] && r.second == true ) {
            KeyreleasedCallback(r.first, nullptr);
        }
    }
    curtain->setVisible(false);
    m->setVisible(false);
    Director::getInstance()->getRunningScene()->getPhysicsWorld()->setSpeed(1);
    this->_eventDispatcher->removeEventListener(pauseListener);
    haveReleased.clear();
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
}
