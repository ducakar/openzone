/*
 *  DynObject.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Object.h"

namespace oz
{

  // dynamic object abstract class
  class DynObject : public Object
  {
    public:

      int     parent;    // index of container object (if object isn't positioned in the world,
                         // it has to be contained in another object, otherwise it will be removed)

      Vec3    velocity;
      Vec3    momentum;  // desired velocity

      float   mass;
      float   lift;

      // if on ground, used as floor normal, it is not set if on another object
      Vec3    floor;
      int     lower;      // index of the lower object
      float   depth;      // how deep under water the object's lower bound is

      explicit DynObject() : parent( -1 ), velocity( Vec3::zero() ), momentum( Vec3::zero() ),
          lower( -1 ), depth( 0.0f )
      {}

      void clearFlags()
      {
        flags &= ~( Object::DISABLED_BIT | Object::ON_FLOOR_BIT | Object::IN_WATER_BIT |
            Object::ON_LADDER_BIT | Object::ON_SLICK_BIT | Object::FRICTING_BIT | Object::HIT_BIT );
        lower = -1;
      }

      virtual void readFull( InputStream *istream );
      virtual void writeFull( OutputStream *ostream ) const;
      virtual void readUpdate( InputStream *istream );
      virtual void writeUpdate( OutputStream *ostream ) const;
  };

}
