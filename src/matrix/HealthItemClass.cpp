/*
 *  HealthItemClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "HealthItemClass.h"

#include "HealthItem.h"
#include "Translator.h"

namespace oz
{

  void HealthItemClass::fill( HealthItemClass *clazz, Config *config )
  {
    DynObjectClass::fill( clazz, config );

    clazz->health  = config->get( "health", 50.0f );
    clazz->stamina = config->get( "stamina", 50.0f );
  }

  ObjectClass *HealthItemClass::init( const String &name, Config *config )
  {
    HealthItemClass *clazz = new HealthItemClass();

    clazz->name  = name;
    clazz->flags = config->get( "flags", DEFAULT_FLAGS ) | BASE_FLAGS;
    fill( clazz, config );

    return clazz;
  }

  Object *HealthItemClass::create( const Vec3 &pos )
  {
    HealthItem *obj = new HealthItem();

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

  Object *HealthItemClass::create( InputStream *istream )
  {
    HealthItem *obj = new HealthItem();

    obj->dim    = dim;
    obj->cell   = null;
    obj->type   = this;

    obj->mass   = mass;
    obj->lift   = lift;

    obj->readFull( istream );

    return obj;
  }

}
