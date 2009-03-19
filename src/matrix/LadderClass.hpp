/*
 *  LadderClass.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *
 *  $Id$
 */

#pragma once

#include "ObjectClass.hpp"
#include "Ladder.hpp"

namespace oz
{

  // TODO LadderClass
  struct LadderClass : ObjectClass
  {
    static Class *init( Config *config );

    Object *create( const Vec3 &pos );
  };

}
