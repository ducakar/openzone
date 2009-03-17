/*
 *  ObjectClass.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *
 *  $Id$
 */

#pragma once

namespace oz
{

  struct ObjectClass
  {
    String  name;

    Vec3    dim;
    int     flags;
    int     type;
    float   damage;
    String  model;

    ObjectClass( const char *file );
  };

}
