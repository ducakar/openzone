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

      void setInWater( const AABB &water )
      {
        flags |= Object::ON_WATER_BIT;

        if( !( oldFlags & Object::ON_WATER_BIT ) ) {
          if( newVelocity.z < -4.0f ) {
            events << new Event( Event::SPLASH_HARD );
          }
          else if( newVelocity.z < -2.0f ) {
            events << new Event( Event::SPLASH_SOFT );
          }
        }
        if( water.includes( p ) ) {
          flags |= Object::UNDER_WATER_BIT;
          flags &= ~Object::ON_FLOOR_BIT;
          lower = -1;
        }
      }

      void setOnLadder()
      {
        flags |= Object::ON_LADDER_BIT;

        flags &= ~Object::ON_FLOOR_BIT;
        lower = -1;
      }

      virtual void load( FILE *stream );
      virtual void save( FILE *stream );
  };

}
