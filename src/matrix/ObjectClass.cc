/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file matrix/ObjectClass.cc
 */

#include "stable.hh"

#include "matrix/ObjectClass.hh"

#include "matrix/Object.hh"
#include "matrix/Library.hh"

namespace oz
{
namespace matrix
{

void ObjectClass::fillCommonConfig( const Config* config )
{
  // suppress warnings
  config->get( "base", "" );

  /*
   * name
   */

  name        = config->get( "name", "" );
  title       = lingua.get( config->get( "title", name ) );
  description = lingua.get( config->get( "description", "" ) );

  if( name.isEmpty() ) {
    throw Exception( "Empty class name" );
  }

  /*
   * dim
   */

  dim.x = config->get( "dim.x", 0.50f );
  dim.y = config->get( "dim.y", 0.50f );
  dim.z = config->get( "dim.z", 0.50f );

  if( dim.x < 0.0f || dim.x > Object::REAL_MAX_DIM ||
      dim.y < 0.0f || dim.y > Object::REAL_MAX_DIM ||
      dim.z < 0.0f )
  {
    throw Exception( "%s: Invalid dimensions. Should be >= 0 and <= 3.99.", name.cstr() );
  }

  if( ( flags & Object::CYLINDER_BIT ) && dim.x != dim.y ) {
    throw Exception( "%s: Cylindric object must have dim.x == dim.y", name.cstr() );
  }

  /*
   * life
   */

  life       = config->get( "life", 100.0f );
  resistance = config->get( "resistance", 100.0f );

  if( life <= 0.0f ) {
    throw Exception( "%s: Invalid life. Should be > 0.", name.cstr() );
  }
  if( resistance < 0.0f ) {
    throw Exception( "%s: Invalid resistance. Should be >= 0.", name.cstr() );
  }

  /*
   * debris
   */

  const char* sFragPool = config->get( "fragPool", "" );

  nFrags   = config->get( "nFrags", 6 );
  fragPool = String::isEmpty( sFragPool ) ? null : library.fragPool( sFragPool );

  /*
   * inventory
   */

  nItems = config->get( "nItems", 0 );

  if( nItems != 0 ) {
    flags |= Object::BROWSABLE_BIT;
  }

  if( nItems < 0 ) {
    throw Exception( "%s: Inventory size must be 0 or a positive integer", name.cstr() );
  }
  if( ( flags & Object::ITEM_BIT ) && nItems != 0 ) {
    throw Exception( "%s: Item cannot have an inventory", name.cstr() );
  }

  // default inventory
  if( nItems != 0 ) {
    defaultItems.alloc( nItems );

    char buffer[] = "item  ";
    for( int i = 0; i < MAX_ITEMS; ++i ) {
      hard_assert( i < 100 );

      buffer[ sizeof( buffer ) - 3 ] = char( '0' + ( i / 10 ) );
      buffer[ sizeof( buffer ) - 2 ] = char( '0' + ( i % 10 ) );

      const char* itemName = config->get( buffer, "" );
      if( !String::isEmpty( itemName ) ) {
        defaultItems.add( library.objClass( itemName ) );
      }
    }

    if( defaultItems.length() > nItems ) {
      throw Exception( "%s: Too many items in the default inventory", name.cstr() );
    }
  }

  /*
   * device
   */

  const char* sDeviceType = config->get( "deviceType", "" );

  if( String::isEmpty( sDeviceType ) ) {
    deviceType = -1;
  }
  else {
    flags |= Object::DEVICE_BIT;

    deviceType = library.deviceIndex( sDeviceType );

    if( flags & Object::USE_FUNC_BIT ) {
      throw Exception( "%s: Device cannot have onUse handler", name.cstr() );
    }
  }

  /*
   * imago
   */

  const char* sImagoType = config->get( "imagoType", "" );

  if( String::isEmpty( sImagoType ) ) {
    imagoType  = -1;
  }
  else {
    flags |= Object::IMAGO_BIT;

    imagoType = library.imagoIndex( sImagoType );

    const char* modelName = config->get( "imagoModel", "" );
    imagoModel = String::isEmpty( modelName ) ? -1 : library.modelIndex( modelName );
  }

  /*
   * audio
   */

  const char* sAudioType = config->get( "audioType", "" );

  if( String::isEmpty( sAudioType ) ) {
    audioType = -1;
  }
  else {
    flags |= Object::AUDIO_BIT;

    audioType = library.audioIndex( sAudioType );

    const char* soundName;
    int         soundIndex;

    soundName  = config->get( "audioSound.create", "" );
    soundIndex = String::isEmpty( soundName ) ? -1 : library.soundIndex( soundName );
    audioSounds[Object::EVENT_CREATE] = soundIndex;

    soundName  = config->get( "audioSound.destroy", "" );
    soundIndex = String::isEmpty( soundName ) ? -1 : library.soundIndex( soundName );
    audioSounds[Object::EVENT_DESTROY] = soundIndex;

    soundName  = config->get( "audioSound.use", "" );
    soundIndex = String::isEmpty( soundName ) ? -1 : library.soundIndex( soundName );
    audioSounds[Object::EVENT_USE] = soundIndex;

    soundName  = config->get( "audioSound.damage", "" );
    soundIndex = String::isEmpty( soundName ) ? -1 : library.soundIndex( soundName );
    audioSounds[Object::EVENT_DAMAGE] = soundIndex;

    soundName  = config->get( "audioSound.hit", "" );
    soundIndex = String::isEmpty( soundName ) ? -1 : library.soundIndex( soundName );
    audioSounds[Object::EVENT_HIT] = soundIndex;
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
}

ObjectClass::~ObjectClass()
{}

ObjectClass* ObjectClass::createClass()
{
  return new ObjectClass();
}

void ObjectClass::initClass( const Config* config )
{
  flags = 0;

  OZ_CLASS_SET_FLAG( Object::DESTROY_FUNC_BIT,   "flag.onDestroy",     true  );
  OZ_CLASS_SET_FLAG( Object::USE_FUNC_BIT,       "flag.onUse",         false );
  OZ_CLASS_SET_FLAG( Object::DAMAGE_FUNC_BIT,    "flag.onDamage",      false );
  OZ_CLASS_SET_FLAG( Object::HIT_FUNC_BIT,       "flag.onHit",         false );
  OZ_CLASS_SET_FLAG( Object::UPDATE_FUNC_BIT,    "flag.onUpdate",      false );
  OZ_CLASS_SET_FLAG( Object::SOLID_BIT,          "flag.solid",         true  );
  OZ_CLASS_SET_FLAG( Object::CYLINDER_BIT,       "flag.cylinder",      true  );
  OZ_CLASS_SET_FLAG( Object::NO_DRAW_BIT,        "flag.noDraw",        false );
  OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,      "flag.wideCull",      false );

  fillCommonConfig( config );
}

Object* ObjectClass::create( int index, const Point3& pos, Heading heading ) const
{
  return new Object( this, index, pos, heading );
}

Object* ObjectClass::create( InputStream* istream ) const
{
  return new Object( this, istream );
}

}
}
