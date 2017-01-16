//
//  main.cpp
//  JTTW
//
//  Created by Bryce Willey on 1/16/17.
//
//

#include <stdio.h>


#include "TestDelegate.h"
#include "cocos2d.h"

using namespace cocos2d;

int main(int argc, char *argv[])
{
    TestDelegate app;
    return Application::getInstance()->run();
}
