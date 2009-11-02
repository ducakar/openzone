/*
 *  Weapon.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Object.h"

namespace oz
{

  struct Weapon : Object
  {
    explicit Weapon( const Vec3 &p = Vec3::zero() );
  };

}
