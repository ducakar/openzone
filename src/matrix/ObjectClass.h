/*
 *  ObjectClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *
 *  $Id$
 */

#pragma once

#include "Object.h"

namespace oz
{

  struct ObjectClass
  {
    typedef ObjectClass *( *InitFunc )( Config *config );

    String name;

    Vec3   dim;
    int    flags;
    int    type;
    float  damage;

    String modelType;
    String modelPath;

    static ObjectClass *init( Config *config );
    virtual Object *create( const Vec3 &pos );
  };

}
