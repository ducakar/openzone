/*
 *  DynamicClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "DynamicClass.h"

#include "Dynamic.h"

namespace oz
{

  ObjectClass *DynamicClass::init( const String &name, const Config *config )
  {
    DynamicClass *clazz = new DynamicClass();

    clazz->name                 = name;
    clazz->description          = config->get( "description", "A Dynamic (dynamic object)" );

    clazz->dim.x                = config->get( "dim.x", 0.50f );
    clazz->dim.y                = config->get( "dim.y", 0.50f );
    clazz->dim.z                = config->get( "dim.z", 0.50f );

    if( clazz->dim.x < 0.0f || clazz->dim.x > AABB::REAL_MAX_DIM ||
        clazz->dim.y < 0.0f || clazz->dim.y > AABB::REAL_MAX_DIM ||
        clazz->dim.z < 0.0f )
    {
      throw Exception( "Invalid object dimensions. Should be >= 0 and <= 3.99." );
    }

    clazz->flags = 0;

    OZ_CLASS_SET_FLAG( Object::DESTROY_FUNC_BIT, "flag.destroyFunc", true  );
    OZ_CLASS_SET_FLAG( Object::DAMAGE_FUNC_BIT,  "flag.damageFunc",  false );
    OZ_CLASS_SET_FLAG( Object::HIT_FUNC_BIT,     "flag.hitFunc",     false );
    OZ_CLASS_SET_FLAG( Object::UPDATE_FUNC_BIT,  "flag.updateFunc",  false );
    OZ_CLASS_SET_FLAG( Object::USE_FUNC_BIT,     "flag.useFunc",     false );
    OZ_CLASS_SET_FLAG( Object::ITEM_BIT,         "flag.item",        false );
    OZ_CLASS_SET_FLAG( Object::CLIP_BIT,         "flag.clip",        true  );
    OZ_CLASS_SET_FLAG( Object::PUSHER_BIT,       "flag.pusher",      false );
    OZ_CLASS_SET_FLAG( Object::HOVER_BIT,        "flag.hover",       false );
    OZ_CLASS_SET_FLAG( Object::NO_DRAW_BIT,      "flag.noDraw",      false );
    OZ_CLASS_SET_FLAG( Object::DELAYED_DRAW_BIT, "flag.delayedDraw", false );
    OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,    "flag.wideCull",    false );

    clazz->life                 = config->get( "life", 100.0f );
    clazz->damageThreshold      = config->get( "damageThreshold", 100.0f );

    if( clazz->life <= 0.0f ) {
      throw Exception( "Invalid object life. Should be > 0." );
    }
    if( clazz->damageThreshold < 0.0f ) {
      throw Exception( "Invalid object damageThreshold. Should be >= 0." );
    }

    clazz->nDebris              = config->get( "nDebris", 8 );
    clazz->debrisVelocitySpread = config->get( "debrisVelocitySpread", 4.0f );
    clazz->debrisRejection      = config->get( "debrisRejection", 1.80f );
    clazz->debrisMass           = config->get( "debrisMass", 0.0f );
    clazz->debrisLifeTime       = config->get( "debrisLifeTime", 2.5f );
    clazz->debrisColor.x        = config->get( "debrisColor.r", 0.5f );
    clazz->debrisColor.y        = config->get( "debrisColor.g", 0.5f );
    clazz->debrisColor.z        = config->get( "debrisColor.b", 0.5f );
    clazz->debrisColorSpread    = config->get( "debrisColorSpread", 0.1f );

    clazz->mass                 = config->get( "mass", 100.0f );
    clazz->lift                 = config->get( "lift", 12.0f );

    if( clazz->mass < 0.1f ) {
      throw Exception( "Invalid object mass. Should be >= 0.1." );
    }
    if( clazz->lift < 0.0f ) {
      throw Exception( "Invalid object lift. Should be >= 0." );
    }

    fillCommon( clazz, config );
    clazz->flags |= BASE_FLAGS;

    return clazz;
  }

  Object *DynamicClass::create( const Vec3 &pos )
  {
    Dynamic *obj = new Dynamic();

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

  Object *DynamicClass::create( InputStream *istream )
  {
    Dynamic *obj = new Dynamic();

    obj->dim    = dim;
    obj->type   = this;

    obj->mass   = mass;
    obj->lift   = lift;

    obj->readFull( istream );

    return obj;
  }

}
