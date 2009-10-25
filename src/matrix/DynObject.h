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
      Vec3    momentum;  // desired velocity

      float   mass;
      float   lift;

      // if on ground, used as floor normal, it is not set if on another object
      Vec3    floor;
      int     lower;      // index of the lower object
      float   waterDepth;

    public:

      explicit DynObject() : velocity( Vec3::zero() ), momentum( Vec3::zero() ), lower( -1 ),
          waterDepth( 0.0f )
      {}

      virtual void readFull( InputStream *istream );
      virtual void writeFull( OutputStream *ostream ) const;
      virtual void readUpdate( InputStream *istream );
      virtual void writeUpdate( OutputStream *ostream ) const;
  };

}
