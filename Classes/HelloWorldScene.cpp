#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include <iostream>

using namespace cocos2d;
using namespace JTTW;

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

    // add a label shows "Hello World"
    // create and initialize a label
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer with Z-order of 0.
    this->addChild(label, 0);

    transformer = new Character("image.png");
    transformer->sprite->setAnchorPoint(Vec2(0.0, 0.0));
    transformer->sprite->setPosition(0, 0);
    this->addChild(transformer->sprite, -2);
    
    auto eventListener = EventListenerKeyboard::create();
    
    eventListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        switch(keyCode) {
            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
                transformer->velocities.x -= 1; // move left
                break;
            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
                transformer->velocities.x += 1; // move right
                break;
            case EventKeyboard::KeyCode::KEY_SPACE:
                transformer->velocities.y = 1;
                transformer->currentState = Character::State::MID_AIR;
            default:
                // do nothing.
                break;
        }
    };
    
    eventListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        switch(keyCode) {
            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
                transformer->velocities.x += 1; // stop moving left
                break;
            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
                transformer->velocities.x -= 1; // stop moving right
                break;
            default:
                // do nothing.
                break;
        }
    };

    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, transformer->sprite);
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
    transformer->move(delta);
}

