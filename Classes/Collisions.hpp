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
    ALL = CollisionCategory::Character | CollisionCategory::Platform // 3
};

}

#endif /* Collisions_h */
