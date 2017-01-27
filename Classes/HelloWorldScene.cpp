#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include <iostream>
#include "LevelParser.hpp"

using namespace cocos2d;
using namespace JTTW;

std::vector<Character *> HelloWorld::characters;
std::deque<AiAgent *> HelloWorld::agents;
AiAgent * HelloWorld::player;
std::vector<Platform> HelloWorld::platforms;
bool HelloWorld::pedestalPopped;
bool HelloWorld::cloudSunk = false;
bool HelloWorld::cloudSinking = false;

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
    auto background = Sprite::create("backgrounds/Sunny Background.png");
    background->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    background->setScale(1.4);
    background->setPosition(0,-300.0);
    //this->addChild(background, -8);
    
    // create menu with the "X" image, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    
    // add menu with Z-order 1.
    this->addChild(menu, 1);

    // Creates the camera, or viewpoint for this scene.
    // 1.7/180.0 means that 1.7 meters in the game world (average human male height) is represented by 180 pixels on screen.
    cocos2d::Layer *layer = cocos2d::Layer::create();
    vp = Viewpoint(visibleSize, 1.7/130.0, layer);
    
    auto rectNode = cocos2d::DrawNode::create();
    cocos2d::Color4F black(1.0, 0.0, 0.0, 1.0);
    cocos2d::Vec2 rect[4];
    rect[0] = cocos2d::Vec2(0.0, 0.0);
    rect[1] = cocos2d::Vec2(100.0, 0.0);
    rect[2] = cocos2d::Vec2(100.0, 100.0);
    rect[3] = cocos2d::Vec2(0.0, 100.0);
    rectNode->drawPolygon(rect, 4, black, 100, black);
    layer->addChild(rectNode, 8);
    
    // Set some simple platforms.
    /*
    cocos2d::Sprite *plat = cocos2d::Sprite::create("platforms/Clear Day.png");
    plat->setAnchorPoint(cocos2d::Vec2(0.0, 0.0));
    plat->setTextureRect(plat->getBoundingBox());
    plat->setPosition(vp.metersToPixels(0.0), vp.metersToPixels(1.5));
    std::cout << "Bounding box: " << plat->getBoundingBox().size.width << ", " << plat->getBoundingBox().size.height << std::endl;
    plat->setContentSize(cocos2d::Size(vp.metersToPixels(3.0), vp.metersToPixels(2.0)));
    std::cout << "Bounding box: " << plat->getBoundingBox().getMaxY() << ", " << plat->getBoundingBox().getMinY() << std::endl;
    layer->addChild(plat, 3);
    platforms.push_back(plat);
    
    plat = cocos2d::Sprite::create("platforms/Clear Day.png");
    plat->setAnchorPoint(cocos2d::Vec2(0.0, 0.0));
    plat->setPosition(vp.metersToPixels(5.0), vp.metersToPixels(3.5));
    plat->setContentSize(cocos2d::Size(vp.metersToPixels(3.0), vp.metersToPixels(2.0)));
    layer->addChild(plat, 3);
    platforms.push_back(plat);
    */

    parseLevelFromJson("demoLevel.json", layer, platforms, vp);
    
    /*Platform lCanyon("platforms/Canyon L.png",
                     cocos2d::Vec2(5.0, 10.0),
                     cocos2d::Size(10.0, 10.0),
                     cocos2d::Vec2(8.5, 9.0), vp);
    layer->addChild(lCanyon.getImage(), 6);
    platforms.push_back(lCanyon);*/
    
    /*
    BadPlatform rCanyon("platforms/Canyon R.png");
    rCanyon.s->setPosition(vp.metersToPixels(14.0), 0.0);
    rCanyon.s->setContentSize(cocos2d::Size(vp.metersToPixels(10.0), vp.metersToPixels(10.0)));
    layer->addChild(rCanyon.s, 6);
    rCanyon.dimensions.setRect(rCanyon.s->getPosition().x, rCanyon.s->getPosition().y, canyonWidth, canyonHeight);
    platforms.push_back(rCanyon);
    
    float cloudHeight = vp.metersToPixels(1.0);
    float cloudWidth = vp.metersToPixels(1.5);
    BadPlatform cloud("platforms/Clear Day.png");
    cloud.s->setPosition(vp.metersToPixels(9.0), vp.metersToPixels(11.0));
    cloud.s->setContentSize(cocos2d::Size(vp.metersToPixels(3.0), vp.metersToPixels(2.0)));
    layer->addChild(cloud.s, 6);
    cloud.dimensions.setRect(cloud.s->getPosition().x + cloudWidth / 2.0,  cloud.s->getPosition().y - 20 /*+ (cloudHeight / 4.0*//*, cloudWidth, cloudHeight);
    platforms.push_back(cloud);
    
    float r1Height = vp.metersToPixels(9.0);
    float r1Width = vp.metersToPixels(3.0);
    BadPlatform r1("platforms/Rock1.png");
    r1.s->setPosition(vp.metersToPixels(25.0), vp.metersToPixels(0.0));
    r1.s->setContentSize(cocos2d::Size(vp.metersToPixels(4.0), vp.metersToPixels(13.0)));
    layer->addChild(r1.s, 6);
    r1.dimensions.setRect(r1.s->getPosition().x,  r1.s->getPosition().y, r1Width, r1Height);
    platforms.push_back(r1);
    
    float r2Height = vp.metersToPixels(11.0);
    float r2Width = vp.metersToPixels(3.0);
    BadPlatform r2("platforms/Rock2.png");
    r2.s->setPosition(vp.metersToPixels(30.0), vp.metersToPixels(0.0));
    r2.s->setContentSize(cocos2d::Size(vp.metersToPixels(4.0), vp.metersToPixels(15.0)));
    layer->addChild(r2.s, 6);
    r2.dimensions.setRect(r2.s->getPosition().x,  r2.s->getPosition().y, r2Width, r2Height);
    platforms.push_back(r2);
    
    float r3Height = vp.metersToPixels(9.0);
    float r3Width = vp.metersToPixels(3.0);
    BadPlatform r3("platforms/Rock3.png");
    r3.s->setPosition(vp.metersToPixels(39.0), vp.metersToPixels(0.0));
    r3.s->setContentSize(cocos2d::Size(vp.metersToPixels(4.0), vp.metersToPixels(13.0)));
    layer->addChild(r3.s, 6);
    r3.dimensions.setRect(r3.s->getPosition().x,  r3.s->getPosition().y, r3Width, r3Height);
    platforms.push_back(r3);
    
    float r4Height = vp.metersToPixels(8.0);
    float r4Width = vp.metersToPixels(3.0);
    BadPlatform r4("platforms/Rock4.png");
    r4.s->setPosition(vp.metersToPixels(46.0), vp.metersToPixels(0.0));
    r4.s->setContentSize(cocos2d::Size(vp.metersToPixels(4.0), vp.metersToPixels(11.0)));
    layer->addChild(r4.s, 6);
    r4.dimensions.setRect(r4.s->getPosition().x,  r4.s->getPosition().y, r4Width, r4Height);
    platforms.push_back(r4);
    
    float pHeight = vp.metersToPixels(1.0);
    float pWidth = vp.metersToPixels(3.0);
    BadPlatform p("platforms/Pedestal.png");
    p.s->setPosition(vp.metersToPixels(52.0), vp.metersToPixels(10.0));
    p.s->setContentSize(cocos2d::Size(vp.metersToPixels(3.0), vp.metersToPixels(1.0)));
    layer->addChild(p.s, 6);
    p.dimensions.setRect(p.s->getPosition().x,  p.s->getPosition().y - pHeight + 5, pWidth, pHeight);
    platforms.push_back(p);
    */
    
    int characterHeight = vp.metersToPixels(1.7);
    
    Character *monkey = new Character("Monkey", JTTW::Rectangle(vp.metersToPixels(1.7), vp.metersToPixels(20.0), characterHeight, characterHeight),
                                     cocos2d::Vec2(vp.metersToPixels(5), vp.metersToPixels(10)), vp.metersToPixels(9.8));
    Character *monk = new Character("Monk", JTTW::Rectangle(vp.metersToPixels(1.7) * 2, vp.metersToPixels(20.0), characterHeight, characterHeight),
                                      cocos2d::Vec2(vp.metersToPixels(3), vp.metersToPixels(6)), vp.metersToPixels(9.8));
    Character *piggy = new Character("Piggy", JTTW::Rectangle(vp.metersToPixels(1.7) * 3, vp.metersToPixels(20.0), characterHeight, characterHeight),
                                      cocos2d::Vec2(vp.metersToPixels(4), vp.metersToPixels(8.7)), vp.metersToPixels(9.8));
    Character *sandy = new Character("sandy", JTTW::Rectangle(vp.metersToPixels(1.7) * 4, vp.metersToPixels(20.0), characterHeight, characterHeight),
                                     cocos2d::Vec2(vp.metersToPixels(4.5), vp.metersToPixels(8)), vp.metersToPixels(9.8));
    characters.push_back(monkey);
    characters.push_back(monk);
    characters.push_back(piggy);
    characters.push_back(sandy);
    
    for (int i = 0; i < characters.size(); i++) {
        Character *body = characters[i];
        //body->ani->setPosition(vp.metersToPixels(1.7) * (i + 1), vp.metersToPixels(20.0));
        layer->addChild(body->ani, i);
        layer->addChild(body->a, -1);
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
    
    this->addChild(layer, 1);
    
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
    for (int i = 0; i < characters.size(); i++) {
        characters[i]->move(delta, platforms);
        
        JTTW::Rectangle a = characters[i]->dimensions;
        cocos2d::Color4F black(0.0, 1.0, 0.0, 1.0);
        cocos2d::Vec2 rect[4];
        rect[0] = cocos2d::Vec2(a.getCenterX() - a.getWidth() / 2.0, a.getCenterY() - a.getHeight() / 2.0);
        rect[1] = cocos2d::Vec2(a.getCenterX() + a.getWidth() / 2.0, a.getCenterY() - a.getHeight() / 2.0);
        rect[2] = cocos2d::Vec2(a.getCenterX() + a.getWidth() / 2.0, a.getCenterY() + a.getHeight() / 2.0);
        rect[3] = cocos2d::Vec2(a.getCenterX() - a.getWidth() / 2.0, a.getCenterY() + a.getHeight() / 2.0);
        characters[i]->a->clear();
        characters[i]->a->drawPolygon(rect, 4, black, 10, black);
        characters[i]->a->clear();
        //rect[1] = cocos2d::Vec2(centerX - collisionWidth / 2.0, centerY - collisionHeight / 2.0);
        //rect[2] = cocos2d::Vec2(centerX + collisionWidth / 2.0, centerY - collisionHeight / 2.0);
        //rect[3] = cocos2d::Vec2(centerX + collisionWidth / 2.0, centerY + collisionHeight / 2.0);
        //rect[4] = cocos2d::Vec2(centerX - collisionWidth / 2.0, centerY + collisionHeight / 2.0);
        characters[i]->a->drawPolygon(rect, 4, black, 10, black);
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

