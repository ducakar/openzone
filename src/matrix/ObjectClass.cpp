/*
 *  ObjectClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "ObjectClass.h"

namespace oz
{

  ObjectClass *ObjectClass::init( Config *config )
  {
    ObjectClass *clazz = new ObjectClass();

    clazz->name = config->get( "name" );

    clazz->dim.x = config->read( "dim.x", 0.5f );
    clazz->dim.y = config->read( "dim.y", 0.5f );
    clazz->dim.z = config->read( "dim.z", 0.5f );

    clazz->flags = config->read( "flags", Object::CLIP_BIT );
    clazz->damage = config->read( "damage", 1.0f );

    clazz->modelType = config->read( "model.type", "MD2" );
    clazz->modelPath = config->read( "model.path", "mdl/goblin.md2" );

    return clazz;
  }

  Object *ObjectClass::create( const Vec3 &pos )
  {
    Object *obj = new Object();

    obj->p = pos;
    obj->dim = dim;

    obj->flags = flags;
    obj->type =this;
    obj->damage = damage;

    return obj;
  }

}
