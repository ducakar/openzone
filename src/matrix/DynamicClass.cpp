/*
 *  DynamicClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/DynamicClass.hpp"

#include "matrix/Dynamic.hpp"

namespace oz
{
namespace matrix
{

ObjectClass* DynamicClass::init( const Config* config )
{
  DynamicClass* clazz = new DynamicClass();

  clazz->flags = Object::DYNAMIC_BIT;

  OZ_CLASS_SET_FLAG( Object::DESTROY_FUNC_BIT,   "flag.onDestroy",     true  );
  OZ_CLASS_SET_FLAG( Object::DAMAGE_FUNC_BIT,    "flag.onDamage",      false );
  OZ_CLASS_SET_FLAG( Object::HIT_FUNC_BIT,       "flag.onHit",         false );
  OZ_CLASS_SET_FLAG( Object::USE_FUNC_BIT,       "flag.onUse",         false );
  OZ_CLASS_SET_FLAG( Object::UPDATE_FUNC_BIT,    "flag.onUpdate",      false );
  OZ_CLASS_SET_FLAG( Object::ITEM_BIT,           "flag.item",          false );
  OZ_CLASS_SET_FLAG( Object::SOLID_BIT,          "flag.solid",         true  );
  OZ_CLASS_SET_FLAG( Object::CYLINDER_BIT,       "flag.cylinder",      true  );
  OZ_CLASS_SET_FLAG( Object::PUSHER_BIT,         "flag.pusher",        false );
  OZ_CLASS_SET_FLAG( Object::NO_DRAW_BIT,        "flag.noDraw",        false );
  OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,      "flag.wideCull",      false );

  clazz->fillCommonConfig( config );

  clazz->mass = config->get( "mass", 100.0f );
  clazz->lift = config->get( "lift", 12.0f );

  if( clazz->mass < 0.01f ) {
    throw Exception( "Invalid object mass. Should be >= 0.01." );
  }
  if( clazz->lift < 0.0f ) {
    throw Exception( "Invalid object lift. Should be >= 0." );
  }

  return clazz;
}

Object* DynamicClass::create( int index, const Point3& pos, Heading heading ) const
{
  Dynamic* obj = new Dynamic();

  hard_assert( obj->index == -1 && obj->cell == null && obj->parent == -1 );

  obj->p          = pos;
  obj->index      = index;
  obj->mass       = mass;
  obj->lift       = lift;

  fillCommonFields( obj );

  obj->flags |= heading;

  if( heading == WEST || heading == EAST ) {
    swap( obj->dim.x, obj->dim.y );
  }

  return obj;
}

Object* DynamicClass::create( int index, InputStream* istream ) const
{
  Dynamic* obj = new Dynamic();

  obj->dim        = dim;
  obj->index      = index;
  obj->clazz      = this;
  obj->resistance = resistance;
  obj->mass       = mass;
  obj->lift       = lift;

  obj->readFull( istream );

  Heading heading = Heading( obj->flags & Object::HEADING_MASK );
  if( heading == WEST || heading == EAST ) {
    swap( obj->dim.x, obj->dim.y );
  }

  return obj;
}

}
}
