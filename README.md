# JTTW
Journey to the West: 2D Game for 460

# Building and Running the Game

## Mac

Open Terminal.app or the equivalent, change to the desired directory, and type the following.

```bash
git clone https://github.com/BrownRiceRice/JTTW 
cd JTTW
git submodule init && git submodule update
cocos2d/download_deps.py # respond to the question with no 
```

Then, open proj.ios\_mac/JTTW.xcodeproj with Xcode. Once open, 
run the "JTTW-desktop target" on "My Mac".

After a long compile time, the game will run. Don't worry though,
when you build again, the compile time will be much faster.

## Explainitation

Journey to the West is built using the cocos2d-x game engine.
As in every cocos2d-x game, a copy of the entire cocos2d library
is required when building and running the game. However, the library
contains many precompiled libraries, headers, and other items that
would generally be excluded by a gitignore.

To solve this, the cocos2d-x library is a submodule, or a link to
another git repository. 
