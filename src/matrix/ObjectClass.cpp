/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/ObjectClass.cpp
 */

#include "stable.hpp"

#include "matrix/ObjectClass.hpp"

#include "matrix/Library.hpp"
#include "matrix/Synapse.hpp"

namespace oz
{
namespace matrix
{

void ObjectClass::fillCommonConfig( const Config* config )
{
  /*
   * name
   */

  name        = config->get( "name", "" );
  title       = gettext( config->get( "title", name ) );
  description = gettext( config->get( "description", "" ) );

  if( name.isEmpty() ) {
    throw Exception( "Empty class name" );
  }

  /*
   * dim
   */

  dim.x = config->get( "dim.x", 0.50f );
  dim.y = config->get( "dim.y", 0.50f );
  dim.z = config->get( "dim.z", 0.50f );

  if( dim.x < 0.0f || dim.x > AABB::REAL_MAX_DIM ||
      dim.y < 0.0f || dim.y > AABB::REAL_MAX_DIM ||
      dim.z < 0.0f )
  {
    throw Exception( "Invalid object dimensions. Should be >= 0 and <= 3.99." );
  }

  if( ( flags & Object::CYLINDER_BIT ) && dim.x != dim.y ) {
    throw Exception( "Cylindric object '" + name + "' should have dim.x == dim.y" );
  }

  /*
   * handler functions
   */

  onDestroy = config->get( "onDestroy", "" );
  onDamage  = config->get( "onDamage", "" );
  onHit     = config->get( "onHit", "" );
  onUse     = config->get( "onUse", "" );
  onUpdate  = config->get( "onUpdate", "" );

  if( !onDestroy.isEmpty() ) {
    flags |= Object::LUA_BIT;

    // disable event handler if explicitly set to false
    if( !config->get( "flag.onDestroy", true ) ) {
      flags &= ~Object::DESTROY_FUNC_BIT;
    }
    else {
      flags |= Object::DESTROY_FUNC_BIT;;
    }
  }
  if( !onDamage.isEmpty() ) {
    flags |= Object::LUA_BIT;

    // disable event handler if explicitly set to false
    if( !config->get( "flag.onDamage", true ) ) {
      flags &= ~Object::DAMAGE_FUNC_BIT;
    }
    else {
      flags |= Object::DAMAGE_FUNC_BIT;
    }
  }
  if( !onHit.isEmpty() ) {
    flags |= Object::LUA_BIT;

    // disable event handler if explicitly set to false
    if( !config->get( "flag.onHit", true ) ) {
      flags &= ~Object::HIT_FUNC_BIT;
    }
    else {
      flags |= Object::HIT_FUNC_BIT;
    }
  }
  if( !onUse.isEmpty() ) {
    flags |= Object::LUA_BIT;

    // disable event handler if explicitly set to false
    if( !config->get( "flag.onUse", true ) ) {
      flags &= ~Object::USE_FUNC_BIT;
    }
    else {
      flags |= Object::USE_FUNC_BIT;
    }
  }
  if( !onUpdate.isEmpty() ) {
    flags |= Object::LUA_BIT;

    // disable event handler if explicitly set to false
    if( !config->get( "flag.onUpdate", true ) ) {
      flags &= ~Object::UPDATE_FUNC_BIT;
    }
    else {
      flags |= Object::UPDATE_FUNC_BIT;
    }
  }

  /*
   * life
   */

  life       = config->get( "life", 100.0f );
  resistance = config->get( "resistance", 100.0f );

  if( life <= 0.0f ) {
    throw Exception( "Invalid object life. Should be > 0." );
  }
  if( resistance < 0.0f ) {
    throw Exception( "Invalid object resistance. Should be >= 0." );
  }

  /*
   * debris
   */

  nDebris              = config->get( "nDebris", 8 );
  debrisVelocitySpread = config->get( "debrisVelocitySpread", 4.0f );
  debrisRejection      = config->get( "debrisRejection", 1.80f );
  debrisMass           = config->get( "debrisMass", 0.0f );
  debrisLifeTime       = config->get( "debrisLifeTime", 2.0f );
  debrisColour.x       = config->get( "debrisColour.r", 0.5f );
  debrisColour.y       = config->get( "debrisColour.g", 0.5f );
  debrisColour.z       = config->get( "debrisColour.b", 0.5f );
  debrisColourSpread   = config->get( "debrisColourSpread", 0.1f );

  /*
   * device
   */

  deviceType = config->get( "deviceType", "" );

  if( !deviceType.isEmpty() ) {
    flags |= Object::DEVICE_BIT;

    if( flags & Object::USE_FUNC_BIT ) {
      throw Exception( "device cannot have onUse handler" );
    }
  }

  /*
   * imago
   */

  imagoType = config->get( "imagoType", "" );

  if( !imagoType.isEmpty() ) {
    flags |= Object::IMAGO_BIT;

    const char* modelName = config->get( "imagoModel", "" );
    imagoModel = modelName[0] == '\0' ? -1 : library.modelIndex( modelName );
  }
  else {
    imagoModel = -1;
  }

  /*
   * audio
   */

  audioType = config->get( "audioType", "" );

  if( !audioType.isEmpty() ) {
    flags |= Object::AUDIO_BIT;

    char buffer[] = "audioSound  ";
    for( int i = 0; i < AUDIO_SOUNDS; ++i ) {
      hard_assert( i < 100 );

      buffer[10] = char( '0' + ( i / 10 ) );
      buffer[11] = char( '0' + ( i % 10 ) );

      const char* soundName = config->get( buffer, "" );
      audioSounds[i] = soundName[0] == '\0' ? -1 : library.soundIndex( soundName );
    }
  }
  else {
    for( int i = 0; i < AUDIO_SOUNDS; ++i ) {
      audioSounds[i] = -1;
    }
  }

  /*
   * inventory
   */

  nItems = config->get( "nItems", 0 );

  if( nItems != 0 ) {
    flags |= Object::BROWSABLE_BIT;
  }

  if( nItems < 0 ) {
    throw Exception( "Inventory size must be 0 or a positive integer" );
  }

  // default inventory
  char buffer[] = "item  ";
  for( int i = 0; i < INVENTORY_ITEMS; ++i ) {
    hard_assert( i < 100 );

    buffer[ sizeof( buffer ) - 3 ] = char( '0' + ( i / 10 ) );
    buffer[ sizeof( buffer ) - 2 ] = char( '0' + ( i % 10 ) );

    String itemName = config->get( buffer, "" );
    if( !itemName.isEmpty() ) {
      items.add( itemName );
    }
  }

  if( ( flags & Object::ITEM_BIT ) && nItems != 0 ) {
    throw Exception( "Object cannot be an item and have an inventory at the same time" );
  }
  if( items.length() > nItems ) {
    throw Exception( "More objects in the default inventory than the inventory size" );
  }
}

void ObjectClass::fillCommonFields( Object* obj ) const
{
  obj->dim        = dim;
  obj->flags      = flags;
  obj->clazz      = this;
  obj->life       = life;
  obj->resistance = resistance;
}

ObjectClass::~ObjectClass()
{}

ObjectClass* ObjectClass::init( const Config* config )
{
  ObjectClass* clazz = new ObjectClass();

  clazz->flags = 0;

  OZ_CLASS_SET_FLAG( Object::DESTROY_FUNC_BIT,   "flag.onDestroy",     true  );
  OZ_CLASS_SET_FLAG( Object::DAMAGE_FUNC_BIT,    "flag.onDamage",      false );
  OZ_CLASS_SET_FLAG( Object::HIT_FUNC_BIT,       "flag.onHit",         false );
  OZ_CLASS_SET_FLAG( Object::USE_FUNC_BIT,       "flag.onUse",         false );
  OZ_CLASS_SET_FLAG( Object::UPDATE_FUNC_BIT,    "flag.onUpdate",      false );
  OZ_CLASS_SET_FLAG( Object::SOLID_BIT,          "flag.solid",         true  );
  OZ_CLASS_SET_FLAG( Object::CYLINDER_BIT,       "flag.cylinder",      true  );
  OZ_CLASS_SET_FLAG( Object::NO_DRAW_BIT,        "flag.noDraw",        false );
  OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,      "flag.wideCull",      false );

  clazz->fillCommonConfig( config );

  return clazz;
}

Object* ObjectClass::create( int index, const Point3& pos, Heading heading ) const
{
  Object* obj = new Object();

  hard_assert( obj->index == -1 && obj->cell == null );

  obj->p          = pos;
  obj->index      = index;

  fillCommonFields( obj );

  obj->flags |= heading;

  if( heading == WEST || heading == EAST ) {
    swap( obj->dim.x, obj->dim.y );
  }

  return obj;
}

Object* ObjectClass::create( int index, InputStream* istream ) const
{
  Object* obj = new Object();

  obj->dim        = dim;
  obj->index      = index;
  obj->clazz      = this;
  obj->resistance = resistance;

  obj->readFull( istream );

  Heading heading = Heading( obj->flags & Object::HEADING_MASK );
  if( heading == WEST || heading == EAST ) {
    swap( obj->dim.x, obj->dim.y );
  }

  return obj;
}

}
}
