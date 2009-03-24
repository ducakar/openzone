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

#include "Class.h"

namespace oz
{

  struct ObjectClass : Class
  {
    String  name;

    Vec3    dim;
    int     flags;
    int     type;
    float   damage;
    String  model;

    static Class *init( Config *config );

    Object *create( const Vec3 &pos );
  };

}
