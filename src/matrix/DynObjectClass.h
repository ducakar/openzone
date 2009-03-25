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
#include "DynObject.h"

namespace oz
{

  struct DynObjectClass : ObjectClass
  {
    float mass;
    float lift;

    static ObjectClass *init( Config *config );
    virtual Object *create( const Vec3 &pos );
  };

}
