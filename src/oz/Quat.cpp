/*
 *  Quat.cpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

/**
 * @file Quat.cpp
 */

#include "Quat.hpp"

namespace oz
{

const Quat Quat::ZERO = Quat( 0.0f, 0.0f, 0.0f, 0.0f );
const Quat Quat::ID   = Quat( 0.0f, 0.0f, 0.0f, 1.0f );

}
