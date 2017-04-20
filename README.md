# Bodhi
A 2D platformer inspired by “Journey to the West”, by Wu Cheng’en.

![Bodhi’s Main Menu](/Bodhi_Menu.png)

Made by Bryce Willey, Mei Tan, and Melinda Crane for the Rice undergraduate course, COMP 460.

# Building and Running the Game

## Mac

Open Terminal.app or the equivalent, change to the desired directory, and type the following.

```bash
git clone https://github.com/BryceStevenWilley/JTTW 
cd JTTW
git submodule init && git submodule update
cocos2d/download_deps.py -r yes
cd Resources/Music && ./download_music.bash && ../../
```

Then, open proj.ios\_mac/JTTW.xcodeproj with Xcode. Once open, 
run the "JTTW-desktop target" on "My Mac".

After a long compile time, the game will run. Don't worry though,
when you build again, the compile time will be much faster.

## Linux

Open a terminal window, and run the following code:

```bash
git clone https://github.com/BryceStevenWilley/JTTW 
cd JTTW
git submodule init && git submodule update
cocos2d/download_deps.py -r yes
cd Resources/Music && ./download_music.bash && ../../
mkdir linux-build && cd linux-build
cmake .. && make -j4
```

## Common Issues

#### “Can’t find cocos2d.h”

Any error similar to this probably means you didn’t run `git submodule init && git submodule update`,
as the compiler can’t find anything in the cocos2d directory. 

#### “error: no such file or directory: ‘/…/JTTW/cocos2d/external/…”

This one means the compiler’s not finding anything in the cocos2d/external folder, which is populated when
you run `cocos2d/download_deps.py`. 


# Credits:

## Programming:

Bryce Willey, Lead Developer
Melinda Crane, [Tooling](https://github.com/runningcrane/JTTWLevelEditor)

[Cocos2d-x](https://github.com/cocos2d/cocos2d-x)
[JSON for Modern C++](https://github.com/nlohmann/json)
[FrozaxShake-v3](https://github.com/tankorsmash/FrozaxShake-v3)

## Art

Mei Tan, Artist and Animator

[Spine](http://esotericsoftware.com/)
[Krita](https://krita.org/en/)

## Music

[CAMeLIA](http://agnello-pecora.chu.jp/CAMeLIA/)
[Hagall](http://hagall.hacca.jp/music)
[Music Egg](http://ontama-m.com/index.html)
[MusMus](http://musmus.main.jp/english.html)
[Timelessberry](http://timelessberry.com/material/)
[Takashiya Takashi/ DOVA-Syndrome](http://dova-s.jp/bgm/play109.html)

## Sound

[Mike Koenig](http://soundbible.com/1221-Pig-Oink.html)
SoundBible.com
Freesound.org
