/*
 *  Dynamic.hpp
 *
 *  Dynamic object
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Object.hpp"

namespace oz
{

  // dynamic object abstract class
  class Dynamic : public Object
  {
    public:

      static Pool<Dynamic> pool;

      int   parent;    // index of container object (if object isn't positioned in the world,
                       // it has to be contained in another object, otherwise it will be removed)

      Vec3  velocity;
      Vec3  momentum;  // desired velocity

      float mass;
      float lift;

      Vec3  floor;      // if on ground, used as floor normal, it is not set if on another object
      int   lower;      // index of the lower object
      float depth;      // how deep under water the object's lower bound is

      explicit Dynamic() : parent( -1 ), velocity( Vec3::ZERO ), momentum( Vec3::ZERO ),
          lower( -1 ), depth( 0.0f )
      {}

      virtual void readFull( InputStream* istream );
      virtual void writeFull( OutputStream* ostream ) const;
      virtual void readUpdate( InputStream* istream );
      virtual void writeUpdate( OutputStream* ostream ) const;

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
