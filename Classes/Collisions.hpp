#ifndef Collisions_h
#define Collisions_h

namespace JTTW {

const int CLIMBEABLE_TAG = 11;
const int MOVEABLE_TAG = 12;
const int VINE_TAG = 13;
const int PROJECTILE_TAG = 14;

enum class CollisionCategory {
    None = 0,
    Character = (1 << 0),  // 1
    Platform = (1 << 1),   // 2
    Boulder = (1 << 2),    // 4
    Projectile = (1 << 4), // 8
    PlatformAndBoulder = Platform |
                         Boulder,
    CharacterAndPlatform = Character |
                           Platform,
    CharacterAndBoulder = Character |
                          Boulder,
    CharacterPlatformAndBoulder = Character |
                                  Platform |
                                  Boulder,
    PlatformBoulderAndProjectile = Platform |
                                   Boulder |
                                   Projectile,
    ALL = Character |
          Platform |
          Boulder |
          Projectile
};

}

#endif /* Collisions_h */
