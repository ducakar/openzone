/*
 *  LadderClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "LadderClass.h"

namespace oz
{

  Class *LadderClass::init( Config *config )
  {
    LadderClass *clazz = new LadderClass();

    clazz->dim.x = config->read( "dim.x", 0.5f );
    clazz->dim.y = config->read( "dim.y", 0.5f );
    clazz->dim.z = config->read( "dim.z", 0.5f );

    clazz->flags = config->read( "flags", Object::DYNAMIC_BIT | Object::CLIP_BIT );
    clazz->type = config->read( "type", 0 );
    clazz->damage = config->read( "damage", 1.0f );

    clazz->model = config->read( "model", "mdl/goblin.md2" );

    return clazz;
  }

  Object *LadderClass::create( const Vec3 &pos )
  {
    Object *obj = new Object();

    obj->p = pos;
    obj->dim = dim;

    obj->flags = flags;
    obj->type = type;
    obj->damage = damage;

    return obj;
  }

}
