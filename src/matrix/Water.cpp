/*
 *  Water.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.hpp"

#include "Water.hpp"

#include "Collider.hpp"
#include "DynObject.hpp"

namespace oz
{

  const char *Water::NAME = "Water";
  const int Water::TYPE = String::hash( Water::NAME );

  Water::Water( const Vec3 &p_, const Vec3 &dim_ )
  {
    p = p_;
    dim = dim_;

    flags = Object::UPDATE_FUNC_BIT | Object::WATER_BIT;
    type = TYPE;

    damage = Math::INF;
  }

  void Water::onUpdate()
  {
    Vector<Object*> objects;
    collider.getOverlaps( *this, &objects, null );

    const int iMax = objects.length();
    for( int i = 0; i < iMax; i++ ) {
      DynObject *obj = (DynObject*) objects[i];

      if( obj->flags & Object::DYNAMIC_BIT ) {
        obj->setInWater( *this );
      }
    }
  }

}
