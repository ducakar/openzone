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

  void DynObjectClass::fill( DynObjectClass *clazz, Config *config )
  {
    ObjectClass::fill( clazz, config );

    clazz->mass = config->get( "mass", 100.0f );
    clazz->lift = config->get( "lift", 12.0f );

    if( clazz->mass < 0.1f ) {
      throw Exception( "Invalid object mass. Should be >= 0.1." );
    }
    if( clazz->lift < 0.0f ) {
      throw Exception( "Invalid object lift. Should be >= 0." );
    }
  }

  ObjectClass *DynObjectClass::init( const String &name, Config *config )
  {
    DynObjectClass *clazz = new DynObjectClass();

    clazz->name  = name;
    clazz->flags = config->get( "flags", DEFAULT_FLAGS ) | BASE_FLAGS;
    fill( clazz, config );

    return clazz;
  }

  Object *DynObjectClass::create( const Vec3 &pos )
  {
    DynObject *obj = new DynObject();

    obj->p        = pos;
    obj->dim      = dim;

    obj->flags    = flags;
    obj->oldFlags = flags;
    obj->type     = this;
    obj->life     = life;

    obj->mass     = mass;
    obj->lift     = lift;

    return obj;
  }

  Object *DynObjectClass::create( InputStream *istream )
  {
    DynObject *obj = new DynObject();

    obj->dim    = dim;
    obj->cell   = null;
    obj->type   = this;

    obj->mass   = mass;
    obj->lift   = lift;

    obj->readFull( istream );

    return obj;
  }

}
