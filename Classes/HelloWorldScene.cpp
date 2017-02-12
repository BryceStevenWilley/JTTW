#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "MainMenuScene.hpp"
#include <iostream>
#include "LevelParser.hpp"
#include "Character.hpp"

using namespace cocos2d;
using namespace JTTW;

//bool HelloWorld::pedestalPopped;
//bool HelloWorld::cloudSunk = false;
//bool HelloWorld::cloudSinking = false;

bool HelloWorld::onContactBegin(cocos2d::PhysicsContact& contact) {
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    
    if (nodeA->getTag() == CHARACTER_TAG) {// && nodeB->getTag() != CHARACTER_TAG) {
        if (nodeA->getPositionY() > nodeB->getPositionY()) {
            // Character landed on a platform, probably.
            Character *c = (Character *)nodeA;
            c->landedCallback();
        }
    }
    if (nodeB->getTag() == CHARACTER_TAG) {// && nodeB->getTag() != CHARACTER_TAG) {
        if (nodeB->getPositionY() > nodeA->getPositionY()) {
            Character *c = (Character *)nodeB;
            c->landedCallback();
        }
    }
    
    //cocos2d::PhysicsJoint *j = cocos2d::PhysicsJointFixed::construct(contact.getShapeA()->getBody(), contact.getShapeB()->getBody(), cocos2d::Vec2(500, 500));
    //this->getScene()->getPhysicsWorld()->addJoint(j);
    return true;
}

bool HelloWorld::onContactEnd(cocos2d::PhysicsContact& contact) {
      auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    
    if (nodeA->getTag() == CHARACTER_TAG) {// && nodeB->getTag() != CHARACTER_TAG) {
        if (nodeA->getPositionY() > nodeB->getPositionY()) {
            // Character landed on a platform, probably.
            Character *c = (Character *)nodeA;
            //c->landedCallback();
            c->rebalanceImpulse();
        }
    }
    if (nodeB->getTag() == CHARACTER_TAG) {// && nodeB->getTag() != CHARACTER_TAG) {
        if (nodeB->getPositionY() > nodeA->getPositionY()) {
            Character *c = (Character *)nodeB;
            //c->landedCallback();
            c->rebalanceImpulse();
        }
    }
    
    //cocos2d::PhysicsJoint *j = cocos2d::PhysicsJointFixed::construct(contact.getShapeA()->getBody(), contact.getShapeB()->getBody(), cocos2d::Vec2(500, 500));
    //this->getScene()->getPhysicsWorld()->addJoint(j);
    return true;
}

Scene* HelloWorld::createScene(std::string levelToLoad) {
    // 'scene' and layer are autorelease objects.
    auto scene = Scene::createWithPhysics();
    //scene->autorelease();
    scene->getPhysicsWorld()->setGravity(cocos2d::Vec2(0, -298));
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    auto layer = HelloWorld::create(levelToLoad);
    scene->addChild(layer);

    return scene;
}

bool HelloWorld::init(std::string levelToLoad) {
    if ( !Layer::init() ) {
        return false;
    }
    
    // aka window dimensions
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add a "close" icon to exit the program. it's an autorelease object
    auto closeItem = cocos2d::MenuItemImage::create("CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));
    
    // draw and add background
    //auto background = cocos2d::Sprite::create("backgrounds/Sunny Background.png");
   // background->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    //background->setScale(1.4);
    //background->setPosition(0,-300.0);
    //this->addChild(background, -8);
    
    // create menu with the "X" image, it's an autorelease object
    auto menu = cocos2d::Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    
    // add menu with Z-order 1.
    this->addChild(menu, 1);

    // Creates the camera, or viewpoint for this scene.
    // 1.7/180.0 means that 1.7 meters in the game world (average human male height) is represented by 180 pixels on screen.
    cocos2d::Layer *layer = cocos2d::Layer::create();
    vp = Viewpoint(visibleSize, 1.7/130.0, layer);

    nlohmann::json characterStart = parseLevelFromJson(levelToLoad, this, layer, platforms, moveables, vp, debugOn);
    int characterHeight = vp.metersToPixels(1.7);

    double monkeyStartX = vp.metersToPixels((double)characterStart["Monkey"]["startingXPos"]);
    double monkeyStartY = vp.metersToPixels((double)characterStart["Monkey"]["startingYPos"]);
        
    double monkStartX = vp.metersToPixels((double)characterStart["Monk"]["startingXPos"]);
    double monkStartY = vp.metersToPixels((double)characterStart["Monk"]["startingYPos"]);
    
    double piggyStartX = vp.metersToPixels((double)characterStart["Piggy"]["startingXPos"]);
    double piggyStartY = vp.metersToPixels((double)characterStart["Piggy"]["startingYPos"]);
    
    double sandyStartX = vp.metersToPixels((double)characterStart["Sandy"]["startingXPos"]);
    double sandyStartY = vp.metersToPixels((double)characterStart["Sandy"]["startingYPos"]);
    
    Character *monkey = new Character("Monkey", PhysicsMaterial(1.0, 0.0, 1.0), cocos2d::Vec2(monkeyStartX, monkeyStartY), cocos2d::Size(characterHeight, characterHeight));
    Character *monk = new Character("Monk", PhysicsMaterial(1.0, 0.0, 1.0), cocos2d::Vec2(monkStartX, monkStartY), cocos2d::Size(characterHeight, characterHeight));
    Character *piggy = new Character("Piggy", PhysicsMaterial(1.0, 0.0, 1.0), cocos2d::Vec2(piggyStartX, piggyStartY), cocos2d::Size(characterHeight, characterHeight));
    Character *sandy = new Character("sandy", PhysicsMaterial(1.0, 0.0, 1.0), cocos2d::Vec2(sandyStartX, sandyStartY), cocos2d::Size(characterHeight, characterHeight));

    characters.push_back(monkey);
    characters.push_back(monk);
    characters.push_back(piggy);
    characters.push_back(sandy);
    
    for (int i = 0; i < characters.size(); i++) {
        Character *body = characters[i];
        layer->addChild(body, i);
        AiAgent *agent = new AiAgent(body);
        agents.push_back(agent);
    }
    
    player = agents[0];

    player->_controlledCharacter->currentCrown->setVisible(true);

    this->addChild(layer, 1);
    
    auto eventListener = cocos2d::EventListenerKeyboard::create();
    
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
                        (*xAgent)->plan(player->_controlledCharacter, characters, keyCode, true);
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
            for (auto xAgent = agents.begin(); xAgent != agents.end(); xAgent++) {
                if ((*xAgent) != player) {
                    (*xAgent)->plan(player->_controlledCharacter, characters, keyCode, false);
                }
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
    auto nextPlayer = agents[charIndex];
    nextPlayer->cedeToPlayer(player);
    player = nextPlayer;
    vp.panToCharacter(player->_controlledCharacter);
}

void HelloWorld::menuCloseCallback(Ref* pSender) {
    auto startScene = MainMenu::createScene();
    cocos2d::Director::getInstance()->replaceScene(startScene);
}

void HelloWorld::update(float delta) {
    for (auto xAgent = agents.begin(); xAgent != agents.end(); xAgent++) {
        if ((*xAgent) != player) {
            (*xAgent)->executePlan(delta);
        }
    }
    
    for (int i = 0; i < characters.size(); i++) {
        if (characters[i]->getPosition().y < -100) { // TODO: un-hardcode this.
            characters[i]->restartFromStart();
        }
    }
    //for (int i = 0; i < characters.size(); i++) {
    //    characters[i]->move(delta, platforms, debugOn);

        // HARDCODED STUFF FOR PEDESTAL DISSAPPEARING
        /*
        if (characters[i]->ani->getPosition().x > vp.metersToPixels(52.0) && !pedestalPopped) {
            // Remove the pedestal from the platforms.
            BadPlatform pedstal = platforms.back();
            platforms.pop_back();
            pedstal.s->runAction(cocos2d::FadeOut::create(2.0));
            pedestalPopped = true;
            characters[i]->ani->setAnimation(0, "fall forwards", false);
            characters[i]->ani->setTimeScale(.3);
        }
        if (characters[i]->characterName == "Piggy" && characters[i]->isDirectlyAbove(platforms[2].dimensions, characters[i]->ani->getPosition(), characters[i]->dimensions) && (!cloudSunk || !cloudSinking)) {
            cloudSinking = true;
        }
         */
    //}
    
    for (int i = 0; i < moveables.size(); i++) {
        moveables[i]->move(delta, false);
    }
    /*
    if (cloudSinking == true) {
        BadPlatform c = platforms[2];
        c.dimensions.setRect(c.dimensions.origin.x, c.dimensions.origin.y - (20 * delta), c.dimensions.size.width, c.dimensions.size.height);
        c.s->setPosition(cocos2d::Vec2(c.s->getPosition().x, c.s->getPosition().y - (20 * delta)));
        if (c.dimensions.getMaxY() < vp.metersToPixels(12)) {
            cloudSinking = false;
            cloudSunk = true;
        }
        platforms[2] = c;
    }
    */
    vp.followCharacter(player->_controlledCharacter, delta);

}

