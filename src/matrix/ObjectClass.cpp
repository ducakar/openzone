/*
 *  ObjectClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/ObjectClass.hpp"

#include "matrix/Object.hpp"
#include "matrix/Translator.hpp"

namespace oz
{

  void ObjectClass::fillCommon( ObjectClass* clazz, const Config* config )
  {
    clazz->description = gettext( clazz->name );

    clazz->onDestroy   = config->get( "onDestroy", "" );
    clazz->onDamage    = config->get( "onDamage", "" );
    clazz->onHit       = config->get( "onHit", "" );
    clazz->onUse       = config->get( "onUse", "" );
    clazz->onUpdate    = config->get( "onUpdate", "" );

    if( !String::isEmpty( clazz->onDestroy ) ) {
      // disable event handler if explicitly set to false
      if( !config->get( "flag.destroyFunc", true ) ) {
        clazz->flags &= ~Object::DESTROY_FUNC_BIT;
      }
      else {
        clazz->flags |= Object::LUA_BIT | Object::DESTROY_FUNC_BIT;;
      }
    }
    if( !String::isEmpty( clazz->onDamage ) ) {
      // disable event handler if explicitly set to false
      if( !config->get( "flag.damageFunc", true ) ) {
        clazz->flags &= ~Object::DAMAGE_FUNC_BIT;
      }
      else {
        clazz->flags |= Object::LUA_BIT | Object::DAMAGE_FUNC_BIT;
      }
    }
    if( !String::isEmpty( clazz->onHit ) ) {
      // disable event handler if explicitly set to false
      if( !config->get( "flag.hitFunc", true ) ) {
        clazz->flags &= ~Object::HIT_FUNC_BIT;
      }
      else {
        clazz->flags |= Object::LUA_BIT | Object::HIT_FUNC_BIT;
      }
    }
    if( !String::isEmpty( clazz->onUse ) ) {
      clazz->flags |= Object::LUA_BIT | Object::USE_FUNC_BIT;

      // disable event handler if explicitly set to false
      if( !config->get( "flag.useFunc", true ) ) {
        clazz->flags &= ~Object::USE_FUNC_BIT;
      }
    }
    if( !String::isEmpty( clazz->onUpdate ) ) {
      // disable event handler if explicitly set to false
      if( !config->get( "flag.updateFunc", true ) ) {
        clazz->flags &= ~Object::UPDATE_FUNC_BIT;
      }
      else {
        clazz->flags |= Object::LUA_BIT | Object::UPDATE_FUNC_BIT;
      }
    }

    clazz->modelType = config->get( "modelType", "" );

    if( !clazz->modelType.isEmpty() ) {
      clazz->flags |= Object::MODEL_BIT;

      const char* modelName = config->get( "modelName", "" );
      clazz->modelIndex = modelName[0] == '\0' ? -1 : translator.modelIndex( modelName );
    }
    else {
      clazz->modelIndex = -1;
    }

    clazz->audioType = config->get( "audioType", "" );

    if( !clazz->audioType.isEmpty() ) {
      clazz->flags |= Object::AUDIO_BIT;

      char buffer[] = "audioSample  ";
      for( int i = 0; i < AUDIO_SAMPLES; ++i ) {
        hard_assert( i < 100 );

        buffer[11] = char( '0' + ( i / 10 ) );
        buffer[12] = char( '0' + ( i % 10 ) );

        const char* sampleName = config->get( buffer, "" );
        clazz->audioSamples[i] = sampleName[0] == '\0' ? -1 : translator.soundIndex( sampleName );
      }
    }
    else {
      for( int i = 0; i < AUDIO_SAMPLES; ++i ) {
        clazz->audioSamples[i] = -1;
      }
    }

    if( ( clazz->flags & Object::CYLINDER_BIT ) && clazz->dim.x != clazz->dim.y ) {
      throw Exception( "Cylindric object '" + clazz->name + "' should have dim.x == dim.y" );
    }
  }

  ObjectClass::~ObjectClass()
  {}

  ObjectClass* ObjectClass::init( const String& name, const Config* config )
  {
    ObjectClass* clazz = new ObjectClass();

    clazz->name                 = name;

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

    OZ_CLASS_SET_FLAG( Object::DESTROY_FUNC_BIT,   "flag.destroyFunc",   true  );
    OZ_CLASS_SET_FLAG( Object::DAMAGE_FUNC_BIT,    "flag.damageFunc",    false );
    OZ_CLASS_SET_FLAG( Object::HIT_FUNC_BIT,       "flag.hitFunc",       false );
    OZ_CLASS_SET_FLAG( Object::USE_FUNC_BIT,       "flag.useFunc",       false );
    OZ_CLASS_SET_FLAG( Object::UPDATE_FUNC_BIT,    "flag.updateFunc",    false );
    OZ_CLASS_SET_FLAG( Object::SOLID_BIT,          "flag.solid",         true  );
    OZ_CLASS_SET_FLAG( Object::CYLINDER_BIT,       "flag.cylinder",      true  );
    OZ_CLASS_SET_FLAG( Object::NO_DRAW_BIT,        "flag.noDraw",        false );
    OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,      "flag.wideCull",      false );
    OZ_CLASS_SET_FLAG( Object::RANDOM_HEADING_BIT, "flag.randomHeading", false );

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
    clazz->debrisLifeTime       = config->get( "debrisLifeTime", 2.0f );
    clazz->debrisColour.x       = config->get( "debrisColour.r", 0.5f );
    clazz->debrisColour.y       = config->get( "debrisColour.g", 0.5f );
    clazz->debrisColour.z       = config->get( "debrisColour.b", 0.5f );
    clazz->debrisColourSpread   = config->get( "debrisColourSpread", 0.1f );

    fillCommon( clazz, config );
    clazz->flags |= BASE_FLAGS;

    return clazz;
  }

  Object* ObjectClass::create( int index, const Point3& pos ) const
  {
    Object* obj = new Object();

    hard_assert( obj->index == -1 && obj->cell == null );

    obj->p        = pos;
    obj->dim      = dim;

    obj->index    = index;
    obj->flags    = flags;
    obj->oldFlags = flags;
    obj->clazz    = this;
    obj->life     = life;

    return obj;
  }

  Object* ObjectClass::create( int index, InputStream* istream ) const
  {
    Object* obj = new Object();

    obj->dim    = dim;

    obj->index  = index;
    obj->clazz  = this;

    obj->readFull( istream );

    return obj;
  }

}
