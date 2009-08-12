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
    DynObjectClass *clazz = new DynObjectClass();

    clazz->name           = name;
    clazz->description    = config->get( "description", "" );

    clazz->dim.x          = config->get( "dim.x", 0.5f );
    clazz->dim.y          = config->get( "dim.y", 0.5f );
    clazz->dim.z          = config->get( "dim.z", 0.5f );

    clazz->flags          = config->get( "flags", DEFAULT_FLAGS ) | BASE_FLAGS;
    clazz->life           = config->get( "life", 100.0f );
    clazz->damageTreshold = config->get( "damageTreshold", 8.0f );
    clazz->damageRatio    = config->get( "damageRatio", 1.0f );

    clazz->mass           = config->get( "mass", 100.0f );
    clazz->lift           = config->get( "lift", 0.10f );

    clazz->modelType      = config->get( "model.type", "MD2" );
    clazz->modelPath      = config->get( "model.path", "mdl/goblin.md2" );

    clazz->audioType      = config->get( "audio.type", "" );

    if( clazz->audioType.length() > 0 ) {
      clazz->flags |= Object::AUDIO_BIT;

      char buffer[] = "audio.sample  ";
      for( int i = 0; i < AUDIO_SAMPLES; i++ ) {
        assert( 0 <= i && i < 100 );

        buffer[ sizeof( buffer ) - 3 ] = '0' + ( i / 10 );
        buffer[ sizeof( buffer ) - 2 ] = '0' + ( i % 10 );

        String sampleName = config->get( buffer, "" );
        clazz->audioSamples[i] = sampleName.length() > 0 ? translator.soundIndex( sampleName ) : -1;
      }
    }

    if( clazz->dim.x < 0.0f || clazz->dim.x > AABB::REAL_MAX_DIM ||
        clazz->dim.y < 0.0f || clazz->dim.y > AABB::REAL_MAX_DIM ||
        clazz->dim.z < 0.0f )
    {
      assert( false );
      throw Exception( 0, "Invalid object dimensions. Should be >= 0 and <= 2.99." );
    }
    if( clazz->life <= 0.0f ) {
      assert( false );
      throw Exception( 0, "Invalid object life. Should be > 0." );
    }
    if( clazz->damageTreshold < 0.0f ) {
      assert( false );
      throw Exception( 0, "Invalid object damageTreshold. Should be >= 0." );
    }
    if( clazz->damageRatio < 0.0f ) {
      assert( false );
      throw Exception( 0, "Invalid object damageRatio. Should be >= 0." );
    }
    if( clazz->mass < 0.0f ) {
      assert( false );
      throw Exception( 0, "Invalid object mass. Should be >= 0." );
    }
    if( clazz->lift < 0.0f ) {
      assert( false );
      throw Exception( 0, "Invalid object lift. Should be >= 0." );
    }
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
    obj->sector = null;
    obj->type   = this;

    obj->mass   = mass;
    obj->lift   = lift;

    obj->readFull( istream );

    return obj;
  }

}
