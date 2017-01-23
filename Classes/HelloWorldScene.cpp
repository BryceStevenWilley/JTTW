#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include <iostream>

using namespace cocos2d;
using namespace JTTW;

std::vector<Character *> HelloWorld::characters;
std::deque<AiAgent *> HelloWorld::agents;
AiAgent * HelloWorld::player;
std::vector<cocos2d::Sprite *> HelloWorld::platforms;
Viewpoint HelloWorld::vp(cocos2d::Size(1.0, 1.0), 1.0, nullptr);

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
    
    std::cout << visibleSize.height << ", " << visibleSize.width << std::endl << std::endl;
    
    // add a "close" icon to exit the program. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));
    
    // draw and add background
    auto background = Sprite::create("backgrounds/Background.png");
    background->setAnchorPoint(Vec2(origin.x, origin.y));
    background->setScale(1.4);
    background->setPosition(0,-300.0);
    this->addChild(background, 0);
    
    // create menu with the "X" image, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    
    // add menu with Z-order 1.
    this->addChild(menu, 1);

    // Creates the camera, or viewpoint for this scene.
    // 1.7/180.0 means that 1.7 meters in the game world (average human male height) is represented by 180 pixels on screen.
    cocos2d::Layer *layer = cocos2d::Layer::create();
    vp = Viewpoint(visibleSize, 1.7/180.0, layer);
    
    // Set some simple platforms.
    cocos2d::Sprite *plat = cocos2d::Sprite::create("platforms/Clear Day.png");
    plat->setAnchorPoint(cocos2d::Vec2(0.0, 0.0));
    plat->setPosition(vp.metersToPixels(0.0), vp.metersToPixels(1.5));
    plat->setContentSize(cocos2d::Size(vp.metersToPixels(3.0), vp.metersToPixels(2.0)));
    layer->addChild(plat, 3);
    platforms.push_back(plat);
    
    plat = cocos2d::Sprite::create("platforms/Clear Day.png");
    plat->setAnchorPoint(cocos2d::Vec2(0.0, 0.0));
    plat->setPosition(vp.metersToPixels(5.0), vp.metersToPixels(3.5));
    plat->setContentSize(cocos2d::Size(vp.metersToPixels(3.0), vp.metersToPixels(2.0)));
    layer->addChild(plat, 3);
    platforms.push_back(plat);
    
    int characterHeight = vp.metersToPixels(1.7);
    
    for (int i = 0; i < 4; i++) {
        Character *body = new Character("Monkey", Vec2(characterHeight, characterHeight),
                                        cocos2d::Vec2(vp.metersToPixels(5), vp.metersToPixels(10)), vp.metersToPixels(9.8));
        body->ani->setPosition(vp.metersToPixels(1.7) * i, 0.0);
        layer->addChild(body->ani, i);
        characters.push_back(body);
        AiAgent *agent = new AiAgent(body);
        agents.push_back(agent);
    }
    
    player = agents.front();
    agents.pop_front();
    
    // Put a marker (the letter 'v') over the active character.
    // TODO: when Viewpoint is finished, just center camera on active character.
    //auto charLabel = Label::createWithTTF("v", "fonts/Marker Felt.ttf", 100);
   // charLabel->setTextColor(cocos2d::Color4B::BLACK);
    //charLabel->setPositionY(vp.metersToPixels(10.4));
    //charLabel->setPositionX(vp.metersToPixels(1.0));
    //(*player)->ani->addChild(charLabel, -5);
    
    this->addChild(layer);
    
    auto eventListener = EventListenerKeyboard::create();
    
    eventListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) mutable {
        switch(keyCode) {
            case EventKeyboard::KeyCode::KEY_TAB:
                std::cout << "Tab pressed" << std::endl;
                {
                    auto nextPlayer = agents.front();
                    agents.pop_front();
                    nextPlayer->cedeToPlayer(player);
                    //(*player)->transferVelocity(*nextPlayer);
                    
                    agents.push_back(player);
                    player = nextPlayer;
                    vp.panToCharacter(player->_controlledCharacter);
                }
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
                    (*xAgent)->plan(player->_controlledCharacter, characters, keyCode, true);
                }
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
                (*xAgent)->plan(player->_controlledCharacter, characters, keyCode, false);
            }
        }
    };

    this->_eventDispatcher->addEventListenerWithFixedPriority(eventListener, 1);
    
    // Look at the Follow action.
    //this->runAction(cocos2d::Follow::create(*player, ))
    this->scheduleUpdate();
    return true;
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
        (*xAgent)->executePlan(delta);
    }
    for (int i = 0; i < 4; i++) {
        characters[i]->move(delta, platforms);
    }
    vp.followCharacter(player->_controlledCharacter, delta);
}

