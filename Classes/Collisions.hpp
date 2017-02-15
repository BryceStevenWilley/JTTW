//
//  Collisions.hpp
//  JTTW
//
//  Created by Bryce Willey on 2/5/17.
//
//

#ifndef Collisions_h
#define Collisions_h

namespace JTTW {

enum class CollisionCategory {
    None = 0,
    Character = (1 << 0), // 1
    Platform = (1 << 1), // 2
    Boulder = (1 << 2), // 4?
    PlatformAndBoulder = CollisionCategory::Platform |
                         CollisionCategory::Boulder,
    CharacterAndBoulder = CollisionCategory::Character |
                          CollisionCategory::Boulder,
    ALL = CollisionCategory::Character |
          CollisionCategory::Platform |
          CollisionCategory::Boulder // 3
};

}

#endif /* Collisions_h */
