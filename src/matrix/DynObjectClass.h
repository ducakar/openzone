/*
 *  DynObjectClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *
 *  $Id$
 */

#pragma once

#include "ObjectClass.h"

namespace oz
{

  struct DynObjectClass : ObjectClass
  {
    float mass;
    float lift;

    int   hitSound;
    int   frictSound;

    static ObjectClass *init( const String &name, Config *config );
    virtual Object *create( const Vec3 &pos );
  };

}
