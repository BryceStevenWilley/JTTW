#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include <iostream>

using namespace cocos2d;
using namespace JTTW;

std::vector<Character *> HelloWorld::characters;
std::vector<Character *>::iterator HelloWorld::player;
std::vector<AiAgent *> HelloWorld::agents;

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
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu with the "X" image, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    Viewpoint vp(visibleSize, 1.7/180.0);
    
    int characterHeight = vp.metersToPixels(1.7);
    
    for (int i = 0; i < 4; i++) {
        Character *body = new Character("Monkey", Vec2(characterHeight, characterHeight),
                                        cocos2d::Vec2(vp.metersToPixels(5), vp.metersToPixels(8)), vp.metersToPixels(9.8));
        body->ani->setPosition(vp.metersToPixels(1.7) * i, 0.0);
        this->addChild(body->ani, i);
        characters.push_back(body);
        AiAgent *agent = new AiAgent(body);
        agents.push_back(agent);
    }
    
    player = characters.begin();
    auto charLabel = Label::createWithTTF("v", "fonts/Marker Felt.ttf", 100);
    charLabel->setPositionY(vp.metersToPixels(27.4));
    charLabel->setPositionX(vp.metersToPixels(1.3));
    (*player)->ani->addChild(charLabel, -5);
    
    auto eventListener = EventListenerKeyboard::create();
    
    eventListener->onKeyPressed = [this, vp](EventKeyboard::KeyCode keyCode, Event* event) {
        switch(keyCode) {
            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
                (*player)->accelerateLeft(1.0);
                break;
            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
                (*player)->accelerateRight(1.0);
                break;
            case EventKeyboard::KeyCode::KEY_SPACE:
                (*player)->jump(1.0);
                break;
            case EventKeyboard::KeyCode::KEY_TAB:
                {
                    std::vector<Character *>::iterator nextPlayer = (player + 1 == characters.end()) ? characters.begin() : player + 1;
                    (*player)->transferVelocity(*nextPlayer);
                    (*player)->ani->removeAllChildren();
                    
                    player = nextPlayer;
                    auto newLabel = Label::createWithTTF("v", "fonts/Marker Felt.ttf", 100);
                    newLabel->setPositionY(vp.metersToPixels(12.4));
                    newLabel->setPositionX(vp.metersToPixels(1.7));
                    (*player)->ani->addChild(newLabel, -5);
                }
                break;
            case EventKeyboard::KeyCode::KEY_ESCAPE:
                this->menuCloseCallback(nullptr); // TODO: should I be using this nullptr?
                break;
            default:
                // do nothing.
                break;
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW ||
            keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW ||
            keyCode == EventKeyboard::KeyCode::KEY_SPACE ||
            keyCode == EventKeyboard::KeyCode::KEY_A ||
            keyCode == EventKeyboard::KeyCode::KEY_O) {
            for (int i = 0; i < 4; i++) {
                agents[i]->plan(*player, characters, keyCode);
            }
        }
    };
    
    eventListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        switch(keyCode) {
            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
                if ((*player)->isMovingLeft()) {
                    (*player)->stop(); // stop moving left
                } else if ((*player)->getXVelocity() == 0.0) {
                    (*player)->accelerateRight(1.0);
                }
                break;
            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
                if ((*player)->isMovingRight()) {
                    (*player)->stop(); // stop moving right
                } else if ((*player)->getXVelocity() == 0.0) {
                    (*player)->accelerateLeft(1.0);
                }
                break;
            default:
                // do nothing.
                break;
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW ||
            keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
            for (int i = 0; i < 4; i++) {
                agents[i]->plan(*player, characters, keyCode);
            }
        }
    };

    this->_eventDispatcher->addEventListenerWithFixedPriority(eventListener, 1);
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
    for (int i = 0; i < 4; i++) {
        agents[i]->executePlan(delta);
         characters[i]->move(delta);
    }
}

