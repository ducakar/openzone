/*
 *  DynObject.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Object.h"

namespace oz
{

  // dynamic object abstract class
  class DynObject : public Object
  {
    public:

      Vec3    velocity;
      Vec3    newVelocity;  // temporary velocity for the next frame

      float   mass;
      float   lift;

      int     lower;        // index of lower object

      // if on ground, used as floor normal, it is not set if on another object
      Vec3    floor;

    public:

      DynObject() : velocity( Vec3::zero() ), newVelocity( Vec3::zero() ), lower( -1 )
      {}

      virtual void load( FILE *stream );
      virtual void save( FILE *stream );
  };

}
