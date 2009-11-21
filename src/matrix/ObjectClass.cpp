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

#include "Translator.h"
#include "Object.h"

namespace oz
{

  ObjectClass::~ObjectClass()
  {}

  void ObjectClass::fillCommon( ObjectClass *clazz, const Config *config )
  {
    clazz->onDestroy            = config->get( "onDestroy", "" );
    clazz->onDamage             = config->get( "onDamage", "" );
    clazz->onHit                = config->get( "onHit", "" );
    clazz->onUpdate             = config->get( "onUpdate", "" );
    clazz->onUse                = config->get( "onUse", "" );

    if( String::length( clazz->onDestroy ) != 0 ) {
      clazz->flags |= Object::DESTROY_FUNC_BIT;
    }
    if( String::length( clazz->onDamage ) != 0 ) {
      clazz->flags |= Object::DAMAGE_FUNC_BIT;
    }
    if( String::length( clazz->onHit ) != 0 ) {
      clazz->flags |= Object::HIT_FUNC_BIT;
    }
    if( String::length( clazz->onUpdate ) != 0 ) {
      clazz->flags |= Object::UPDATE_FUNC_BIT;
    }
    if( String::length( clazz->onUse ) != 0 ) {
      clazz->flags |= Object::USE_FUNC_BIT;
    }

    clazz->modelType            = config->get( "modelType", "" );
    clazz->modelName            = config->get( "modelName", "" );

    if( clazz->modelType.length() > 0 ) {
      clazz->flags |= Object::MODEL_BIT;
    }

    clazz->audioType            = config->get( "audioType", "" );

    if( clazz->audioType.length() > 0 ) {
      clazz->flags |= Object::AUDIO_BIT;

      char buffer[] = "audioSample  ";
      for( int i = 0; i < AUDIO_SAMPLES; i++ ) {
        assert( 0 <= i && i < 100 );

        buffer[ sizeof( buffer ) - 3 ] = '0' + ( i / 10 );
        buffer[ sizeof( buffer ) - 2 ] = '0' + ( i % 10 );

        String sampleName = config->get( buffer, "" );
        clazz->audioSamples[i] = sampleName.length() > 0 ? translator.soundIndex( sampleName ) : -1;
      }
    }
  }

  ObjectClass *ObjectClass::init( const String &name, const Config *config )
  {
    ObjectClass *clazz = new ObjectClass();

    clazz->name                 = name;
    clazz->description          = config->get( "description", "" );

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
    OZ_CLASS_SET_FLAG( Object::BLEND_BIT,        "flag.blend",       false );
    OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,    "flag.wideCull",    false );

    clazz->life                 = config->get( "life", 100.0f );
    clazz->damageTreshold       = config->get( "damageTreshold", 100.0f );
    clazz->damageRatio          = config->get( "damageRatio", 1.0f );

    if( clazz->life <= 0.0f ) {
      throw Exception( "Invalid object life. Should be > 0." );
    }
    if( clazz->damageTreshold < 0.0f ) {
      throw Exception( "Invalid object damageTreshold. Should be >= 0." );
    }
    if( clazz->damageRatio < 0.0f ) {
      throw Exception( "Invalid object damageRatio. Should be >= 0." );
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

  Object *ObjectClass::create( const Vec3 &pos )
  {
    Object *obj = new Object();

    obj->p        = pos;
    obj->dim      = dim;

    obj->flags    = flags;
    obj->oldFlags = flags;
    obj->type     = this;
    obj->life     = life;

    return obj;
  }

  Object *ObjectClass::create( InputStream *istream )
  {
    Object *obj = new Object();

    obj->dim    = dim;
    obj->type   = this;

    obj->readFull( istream );

    return obj;
  }

}
