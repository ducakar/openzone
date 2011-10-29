/*
 *  common.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

/**
 * @file common/common.cpp
 */

#include "common/common.hpp"

namespace oz
{

// ensure epsilon is big enough for a 4 km x 4 km world (1 mm should do)
// EPSILON = Orbis::DIM * 4.0f * Math::EPSILON
const float EPSILON = 2048.0f * 4.0f * Math::EPSILON;

}
