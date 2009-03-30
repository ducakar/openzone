/*
 *  DynObjectClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "DynObjectClass.h"

namespace oz
{

  ObjectClass *DynObjectClass::init( Config *config )
  {
    // TODO add class name
    DynObjectClass *clazz = new DynObjectClass();

    clazz->name = config->get( "name", "NONAME" );

    clazz->dim.x = config->get( "dim.x", 0.5f );
    clazz->dim.y = config->get( "dim.y", 0.5f );
    clazz->dim.z = config->get( "dim.z", 0.5f );

    clazz->flags = config->get( "flags", Object::DYNAMIC_BIT | Object::CLIP_BIT );
    clazz->damage = config->get( "damage", 1.0f );

    clazz->mass = config->get( "mass", 10.0f );
    clazz->lift = config->get( "lift", 0.03f );

    clazz->modelType = config->get( "model.type", "MD2" );
    clazz->modelPath = config->get( "model.path", "mdl/goblin.md2" );

    return clazz;
  }

  Object *DynObjectClass::create( const Vec3 &pos )
  {
    DynObject *obj = new DynObject();

    obj->p = pos;
    obj->dim = dim;

    obj->flags = flags;
    obj->type = this;
    obj->damage = damage;

    obj->mass = mass;
    obj->lift = lift;

    return obj;
  }

}
