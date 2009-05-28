/*
 *  ObjectClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "ObjectClass.h"

#include "Translator.h"

namespace oz
{

  ObjectClass::~ObjectClass()
  {}

  ObjectClass *ObjectClass::init( const String &name, Config *config )
  {
    ObjectClass *clazz = new ObjectClass();

    clazz->name = name;
    clazz->description = config->get( "description", "" );

    clazz->dim.x = config->get( "dim.x", 0.5f );
    clazz->dim.y = config->get( "dim.y", 0.5f );
    clazz->dim.z = config->get( "dim.z", 0.5f );

    clazz->flags = config->get( "flags", Object::CLIP_BIT );
    clazz->damage = config->get( "damage", 1.0f );

    clazz->modelType = config->get( "model.type", "MD2" );
    clazz->modelPath = config->get( "model.path", "mdl/goblin.md2" );

    clazz->audioType = config->get( "audio.type", "SimpleAudio" );

    char buffer[] = "audio.sample  ";
    for( int i = 0; i < AUDIO_SAMPLES; i++ ) {
      assert( 0 <= i && i < 100 );

      buffer[ sizeof( buffer ) - 2 ] = i / 10;
      buffer[ sizeof( buffer ) - 1 ] = i % 10;

      clazz->audioSamples[i] = translator.soundIndex( config->get( buffer, "" ) );
    }
    return clazz;
  }

  Object *ObjectClass::create( const Vec3 &pos )
  {
    Object *obj = new Object();

    obj->p = pos;
    obj->dim = dim;

    obj->flags = flags;
    obj->oldFlags = flags;
    obj->type =this;
    obj->damage = damage;

    return obj;
  }

}
