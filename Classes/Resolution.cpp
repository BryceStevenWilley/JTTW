//
//  Resolution.cpp
//  JTTW
//
//  Created by Bryce Willey on 3/21/17.
//
//

#include "Resolution.hpp"

using namespace JTTW;

// NOTE: all resolutions are the same aspect ratio (16:9)
double JTTW::ideal2Res(double ideal) {
    return ideal * screenScale;
}
