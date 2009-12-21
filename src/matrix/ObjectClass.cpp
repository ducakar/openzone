/*
 *  ObjectClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "ObjectClass.h"

#include "Object.h"
#include "Translator.h"

namespace oz
{

  ObjectClass::~ObjectClass()
  {}

  void ObjectClass::fillCommon( ObjectClass *clazz, const Config *config )
  {
    clazz->onCreate             = config->get( "onCreate", "" );
    clazz->onDestroy            = config->get( "onDestroy", "" );
    clazz->onDamage             = config->get( "onDamage", "" );
    clazz->onHit                = config->get( "onHit", "" );
    clazz->onUpdate             = config->get( "onUpdate", "" );
    clazz->onUse                = config->get( "onUse", "" );

    if( !String::isEmpty( clazz->onCreate ) ) {
      clazz->flags |= Object::LUA_BIT | Object::CREATE_FUNC_BIT;

      // disable event handler if explicitly set to false
      if( !config->get( "flag.createFunc", true ) ) {
        clazz->flags &= ~Object::CREATE_FUNC_BIT;
      }
    }
    if( !String::isEmpty( clazz->onDestroy ) ) {
      clazz->flags |= Object::LUA_BIT | Object::DESTROY_FUNC_BIT;

      // disable event handler if explicitly set to false
      if( !config->get( "flag.destroyFunc", true ) ) {
        clazz->flags &= ~Object::DESTROY_FUNC_BIT;
      }
    }
    if( !String::isEmpty( clazz->onDamage ) ) {
      clazz->flags |= Object::LUA_BIT | Object::DAMAGE_FUNC_BIT;

      // disable event handler if explicitly set to false
      if( !config->get( "flag.damageFunc", true ) ) {
        clazz->flags &= ~Object::DAMAGE_FUNC_BIT;
      }
    }
    if( !String::isEmpty( clazz->onHit ) ) {
      clazz->flags |= Object::LUA_BIT | Object::HIT_FUNC_BIT;

      // disable event handler if explicitly set to false
      if( !config->get( "flag.hitFunc", true ) ) {
        clazz->flags &= ~Object::HIT_FUNC_BIT;
      }
    }
    if( !String::isEmpty( clazz->onUpdate ) ) {
      clazz->flags |= Object::LUA_BIT | Object::UPDATE_FUNC_BIT;

      // disable event handler if explicitly set to false
      if( !config->get( "flag.updateFunc", true ) ) {
        clazz->flags &= ~Object::UPDATE_FUNC_BIT;
      }
    }
    if( !String::isEmpty( clazz->onUse ) ) {
      clazz->flags |= Object::LUA_BIT | Object::USE_FUNC_BIT;

      // disable event handler if explicitly set to false
      if( !config->get( "flag.useFunc", true ) ) {
        clazz->flags &= ~Object::USE_FUNC_BIT;
      }
    }

    clazz->modelType            = config->get( "modelType", "" );
    clazz->modelName            = config->get( "modelName", "" );

    if( !clazz->modelType.isEmpty() ) {
      clazz->flags |= Object::MODEL_BIT;
    }

    clazz->audioType            = config->get( "audioType", "" );

    if( !clazz->audioType.isEmpty() ) {
      clazz->flags |= Object::AUDIO_BIT;

      char buffer[] = "audioSample  ";
      for( int i = 0; i < AUDIO_SAMPLES; i++ ) {
        assert( 0 <= i && i < 100 );

        buffer[ sizeof( buffer ) - 3 ] = '0' + ( i / 10 );
        buffer[ sizeof( buffer ) - 2 ] = '0' + ( i % 10 );

        String sampleName = config->get( buffer, "" );
        clazz->audioSamples[i] = sampleName.isEmpty() ? -1 : translator.soundIndex( sampleName );
      }
    }
  }

  ObjectClass *ObjectClass::init( const String &name, const Config *config )
  {
    ObjectClass *clazz = new ObjectClass();

    clazz->name                 = name;
    clazz->description          = config->get( "description", name );

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
    OZ_CLASS_SET_FLAG( Object::CLIP_BIT,         "flag.clip",        true  );
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

    fillCommon( clazz, config );
    clazz->flags |= BASE_FLAGS;

    return clazz;
  }

  Object *ObjectClass::create( int index, const Vec3 &pos )
  {
    Object *obj = new Object();

    assert( obj->index == -1 && obj->cell == null );

    obj->p        = pos;
    obj->dim      = dim;

    obj->index    = index;
    obj->flags    = flags;
    obj->oldFlags = flags;
    obj->type     = this;
    obj->life     = life;

    return obj;
  }

  Object *ObjectClass::create( int index, InputStream *istream )
  {
    Object *obj = new Object();

    obj->dim    = dim;

    obj->index  = index;
    obj->type   = this;

    obj->readFull( istream );

    return obj;
  }

}
