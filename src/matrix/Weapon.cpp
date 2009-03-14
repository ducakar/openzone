/*
 *  Weapon.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.hpp"

#include "Weapon.hpp"

namespace oz
{

  Weapon::Weapon( const Vec3 &p_ )
  {
    p = p_,
    dim = Vec3( 0.3f, 0.3f, 0.3f );

    flags = 0;
    type = 0;

    damage = Math::INF;
  }
}
