#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include <iostream>
#include "LevelParser.hpp"

using namespace cocos2d;
using namespace JTTW;

//std::vector<Character *> HelloWorld::characters;
//std::vector<AiAgent *> HelloWorld::agents;
//AiAgent * HelloWorld::player;
//std::vector<GameObject *> HelloWorld::platforms;
//std::vector<MoveablePlatform *> HelloWorld::moveables;

//bool HelloWorld::debugOn = true;

//bool HelloWorld::pedestalPopped;
//bool HelloWorld::cloudSunk = false;
//bool HelloWorld::cloudSinking = false;

//Viewpoint HelloWorld::vp(cocos2d::Size(1.0, 1.0), 1.0, nullptr);

Scene* HelloWorld::createScene() {
    // 'scene' and layer are autorelease objects.
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    scene->addChild(layer);

    return scene;
}

bool HelloWorld::init() {
    if ( !Layer::init() ) {
        return false;
    }
    
    // aka window dimensions
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add a "close" icon to exit the program. it's an autorelease object
    auto closeItem = MenuItemImage::create("CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));
    
    // draw and add background
    auto background = Sprite::create("backgrounds/Sunny Background.png");
    background->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    background->setScale(1.4);
    background->setPosition(0,-300.0);
    this->addChild(background, -8);
    
    // create menu with the "X" image, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    
    // add menu with Z-order 1.
    this->addChild(menu, 1);

    // Creates the camera, or viewpoint for this scene.
    // 1.7/180.0 means that 1.7 meters in the game world (average human male height) is represented by 180 pixels on screen.
    cocos2d::Layer *layer = cocos2d::Layer::create();
    vp = Viewpoint(visibleSize, 1.7/130.0, layer);

    nlohmann::json characterStart = parseLevelFromJson("SimpleDitch.json", layer, platforms, moveables, vp, debugOn);
    int characterHeight = vp.metersToPixels(1.7);
    double monkeyStartX = vp.metersToPixels((double)characterStart["Monkey"][0]);
    double monkeyStartY = vp.metersToPixels((double)characterStart["Monkey"][1]);
    Character *monkey = new Character(
            "Monkey",
            JTTW::Rectangle(monkeyStartX, monkeyStartY,characterHeight, characterHeight),
            cocos2d::Vec2(vp.metersToPixels(5), vp.metersToPixels(10)), 60, vp.metersToPixels(9.8));
    
    double monkStartX = vp.metersToPixels((double)characterStart["Monk"][0]);
    double monkStartY = vp.metersToPixels((double)characterStart["Monk"][1]);
    Character *monk = new Character(
            "Monk",
            JTTW::Rectangle(monkStartX, monkStartY, characterHeight, characterHeight),
            cocos2d::Vec2(vp.metersToPixels(3), vp.metersToPixels(6)), 50, vp.metersToPixels(9.8));
    
    double piggyStartX = vp.metersToPixels((double)characterStart["Piggy"][0]);
    double piggyStartY = vp.metersToPixels((double)characterStart["Piggy"][1]);
    Character *piggy = new Character(
            "Piggy",
            JTTW::Rectangle(piggyStartX, piggyStartY, characterHeight, characterHeight),
            cocos2d::Vec2(vp.metersToPixels(4), vp.metersToPixels(8.7)), 150, vp.metersToPixels(9.8));
    
    double sandyStartX = vp.metersToPixels((double)characterStart["Sandy"][0]);
    double sandyStartY = vp.metersToPixels((double)characterStart["Sandy"][1]);
    Character *sandy = new Character(
           "sandy",
            JTTW::Rectangle(sandyStartX, sandyStartY, characterHeight, characterHeight),
            cocos2d::Vec2(vp.metersToPixels(4.5), vp.metersToPixels(8)), 100, vp.metersToPixels(9.8));
    
    characters.push_back(monkey);
    characters.push_back(monk);
    characters.push_back(piggy);
    characters.push_back(sandy);
    
    for (int i = 0; i < characters.size(); i++) {
        Character *body = characters[i];
        layer->addChild(body->ani, i);
        layer->addChild(body->collisionBoxNode, -1);
        AiAgent *agent = new AiAgent(body);
        agents.push_back(agent);
    }
    
    player = agents[0];
    //agents.pop_front();
    
    this->addChild(layer, 1);
    
    auto eventListener = EventListenerKeyboard::create();
    
    eventListener->onKeyPressed = [this, layer](EventKeyboard::KeyCode keyCode, Event* event) mutable {
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
            case EventKeyboard::KeyCode::KEY_TAB:
                // Try to restart!
                /*
                layer->removeAllChildren();
                this->removeAllChildren();
                agents.clear();
                characters.clear();
                platforms.clear();
                moveables.clear();
                this->init();
                 */
                //{
                    // Cleanup all characters.
                    //layer->removeAllChildrenWithCleanup(true);
                    //for (auto ch = characters.begin(); ch != characters.end(); ch++) {
                        //
                    //}
                    //auto newScene = HelloWorld::createScene();
                    //Director::getInstance()->replaceScene(newScene);
                //}
                break;
                
            default:
                // do nothing.
                break;
        }
    };
    
    eventListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW ||
            keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
            player->plan(characters, keyCode, false);
            for (auto xAgent = agents.begin(); xAgent != agents.end(); xAgent++) {
                if ((*xAgent) != player) {
                    (*xAgent)->plan(player->_controlledCharacter, characters, keyCode, false);
                }
            }
        }
    };

    this->_eventDispatcher->addEventListenerWithFixedPriority(eventListener, 1);
    
    // Look at the Follow action.
    //this->runAction(cocos2d::Follow::create(*player, ))
    this->scheduleUpdate();
    return true;
}

// TODO: maybe switch this to a map, so we don't need that '!= player' bit.
void HelloWorld::switchToCharacter(int charIndex) {
    auto nextPlayer = agents[charIndex];
    nextPlayer->cedeToPlayer(player);
        
    //agents.push_back(player);
    player = nextPlayer;
    vp.panToCharacter(player->_controlledCharacter);
}

void HelloWorld::menuCloseCallback(Ref* pSender) {
    // Close the game and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        exit(0);
    #endif
}

void HelloWorld::update(float delta) {
    for (auto xAgent = agents.begin(); xAgent != agents.end(); xAgent++) {
        if ((*xAgent) != player) {
            (*xAgent)->executePlan(delta);
        }
    }
    for (int i = 0; i < characters.size(); i++) {
        characters[i]->move(delta, platforms, debugOn);

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
    }
    
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

