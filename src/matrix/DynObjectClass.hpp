/*
 *  DynObjectClass.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *
 *  $Id$
 */

#pragma once

#include "ObjectClass.hpp"
#include "DynObject.hpp"

namespace oz
{

  struct DynObjectClass : ObjectClass
  {
    float mass;
    float lift;

    static Class *init( Config *config );

    Object *create( const Vec3 &pos );
  };

}
