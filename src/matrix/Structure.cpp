/*
 *  Structure.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.hpp"

#include "Structure.hpp"

namespace oz
{

  Structure::Structure() {
  }

  Structure::Structure( const Vec3 &p_, int bsp_ ) : p( p_ ), bsp( bsp_ )
  {}

}
