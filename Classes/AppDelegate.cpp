#include "AppDelegate.h"
#include "MainMenuScene.hpp"
#include "SimpleAudioEngine.h"
#include "Resolution.hpp"

using namespace cocos2d;

AppDelegate::AppDelegate() {
}

AppDelegate::~AppDelegate() {
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs() {
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages() {
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        /*
         * Screen size / resolution changed here. See https://www.raywenderlich.com/95835/cocos2d-x-tutorial-beginners
         */
        glview = GLViewImpl::createWithRect("Bodhi", Rect(0, 0, JTTW::actualResolution.width, JTTW::actualResolution.height), 1.0);
#else
        glview = GLViewImpl::create("Not WIN/MAC/LINUX");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    //director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    // Don't set design resolution since we're full screening. See if that's an issue.
    //glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    auto frameSize = glview->getFrameSize();
    // if the frame's height is larger than the height of medium size.
    if (frameSize.height > JTTW::mediumResolution.height) {
        director->setContentScaleFactor(MIN(JTTW::largeResolution.height/JTTW::designResolution.height, JTTW::largeResolution.width/JTTW::designResolution.width));
    }
    // if the frame's height is larger than the height of small size.
    else if (frameSize.height > JTTW::smallResolution.height) {
        director->setContentScaleFactor(MIN(JTTW::mediumResolution.height/JTTW::designResolution.height, JTTW::mediumResolution.width/JTTW::designResolution.width));
    }
    // if the frame's height is smaller than the height of medium size.
    else {
        director->setContentScaleFactor(MIN(JTTW::smallResolution.height/JTTW::designResolution.height, JTTW::smallResolution.width/JTTW::designResolution.width));
    }

    register_all_packages();

    // create a scene. it's an autorelease object
    auto scene = JTTW::MainMenu::createScene();

    // run
    director->runWithScene(scene);
    
    // Audio!
    auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
    audio->preloadBackgroundMusic("Music/MenuScreen.mp3");
    audio->playBackgroundMusic("Music/MenuScreen.mp3", true);
    
    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be paused
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
