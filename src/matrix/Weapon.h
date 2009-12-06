/*
 *  Weapon.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "DynObject.h"

namespace oz
{

  struct Weapon : DynObject
  {
    explicit Weapon( const Vec3 &p = Vec3::zero() );
  };

}
