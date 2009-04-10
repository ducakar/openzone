/*
 *  DynObjectClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "DynObjectClass.h"

#include "DynObject.h"
#include "Translator.h"

namespace oz
{

  ObjectClass *DynObjectClass::init( const String &name, Config *config )
  {
    // TODO add class name
    DynObjectClass *clazz = new DynObjectClass();

    clazz->name = name;
    clazz->description = config->get( "description", "" );

    clazz->dim.x = config->get( "dim.x", 0.5f );
    clazz->dim.y = config->get( "dim.y", 0.5f );
    clazz->dim.z = config->get( "dim.z", 0.5f );

    clazz->flags = config->get( "flags", Object::CLIP_BIT ) | Object::DYNAMIC_BIT;
    clazz->damage = config->get( "damage", 1.0f );

    clazz->mass = config->get( "mass", 100.0f );
    clazz->lift = config->get( "lift", 0.10f );

    clazz->modelType = config->get( "model.type", "MD2" );
    clazz->modelPath = config->get( "model.path", "mdl/goblin.md2" );

    const String &sHitSound   = config->get( "sound.hit", "" );
    const String &sFrictSound = config->get( "sound.friction", "" );

    clazz->hitSound   = sHitSound.length() > 0   ? translator.soundIndex( sHitSound )   : -1;
    clazz->frictSound = sFrictSound.length() > 0 ? translator.soundIndex( sFrictSound ) : -1;

    return clazz;
  }

  Object *DynObjectClass::create( const Vec3 &pos )
  {
    DynObject *obj = new DynObject();

    obj->p = pos;
    obj->dim = dim;

    obj->flags = flags;
    obj->oldFlags = flags;
    obj->type = this;
    obj->damage = damage;

    obj->mass = mass;
    obj->lift = lift;

    return obj;
  }

}
