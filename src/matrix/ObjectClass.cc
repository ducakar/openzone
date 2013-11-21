/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <matrix/ObjectClass.hh>

#include <common/Lingua.hh>
#include <matrix/Object.hh>
#include <matrix/Liber.hh>

namespace oz
{

ObjectClass::~ObjectClass()
{}

ObjectClass* ObjectClass::createClass()
{
  return new ObjectClass();
}

void ObjectClass::init( const JSON& config, const char* name_ )
{
  /*
   * name
   */

  name        = name_;
  title       = config["title"].get( name );
  description = config["description"].get( "" );

  if( name.isEmpty() ) {
    OZ_ERROR( "Empty class name" );
  }

  /*
   * flags
   */

  flags = 0;

  OZ_CLASS_FLAG( Object::DESTROY_FUNC_BIT, "flag.onDestroy", true  );
  OZ_CLASS_FLAG( Object::USE_FUNC_BIT,     "flag.onUse",     false );
  OZ_CLASS_FLAG( Object::UPDATE_FUNC_BIT,  "flag.onUpdate",  false );
  OZ_CLASS_FLAG( Object::STATUS_FUNC_BIT,  "flag.getStatus", false );
  OZ_CLASS_FLAG( Object::SOLID_BIT,        "flag.solid",     true  );
  OZ_CLASS_FLAG( Object::CYLINDER_BIT,     "flag.cylinder",  true  );
  OZ_CLASS_FLAG( Object::WIDE_CULL_BIT,    "flag.wideCull",  false );

  /*
   * dim
   */

  dim = config["dim"].get( Vec3( -1.0f, -1.0f, -1.0f ) );

  if( dim.x < 0.0f || dim.x > Object::REAL_MAX_DIM ||
      dim.y < 0.0f || dim.y > Object::REAL_MAX_DIM ||
      dim.z < 0.0f )
  {
    OZ_ERROR( "%s: Invalid dimensions. Should be >= 0 and <= %g.", name_, Object::REAL_MAX_DIM );
  }

  if( ( flags & Object::CYLINDER_BIT ) && dim.x != dim.y ) {
    OZ_ERROR( "%s: Cylindrical object must have dim.x == dim.y", name_ );
  }

  /*
   * life
   */

  life       = config["life"].get( 0.0f );
  resistance = config["resistance"].get( 100.0f );

  if( life <= 0.0f || !Math::isFinite( life ) ) {
    OZ_ERROR( "%s: Invalid life value. Should be > 0 and finite. If you want infinite life rather"
              " set resistance to infinity (\"inf\").", name_ );
  }
  if( resistance < 0.0f ) {
    OZ_ERROR( "%s: Invalid resistance. Should be >= 0.", name_ );
  }

  /*
   * attributes
   */

  attributes = 0;

  OZ_CLASS_ATTRIB( ObjectClass::NIGHT_VISION_BIT, "attrib.nightVision", false );
  OZ_CLASS_ATTRIB( ObjectClass::BINOCULARS_BIT,   "attrib.binoculars",  false );
  OZ_CLASS_ATTRIB( ObjectClass::GALILEO_BIT,      "attrib.galileo",     false );
  OZ_CLASS_ATTRIB( ObjectClass::MUSIC_PLAYER_BIT, "attrib.musicPlayer", false );
  OZ_CLASS_ATTRIB( ObjectClass::SUICIDE_BIT,      "attrib.suicide",     false );

  /*
   * key code
   */

  key = config["key"].get( 0 );

  /*
   * cost
   */

  cost = config["cost"].get( 0 );

  /*
   * inventory
   */

  nItems = config["nItems"].get( 0 );
  defaultItems.clear();

  if( nItems != 0 ) {
    flags |= Object::BROWSABLE_BIT;
  }

  if( nItems < 0 ) {
    OZ_ERROR( "%s: Inventory size must be a non-negative integer", name_ );
  }
  if( ( flags & Object::ITEM_BIT ) && nItems != 0 ) {
    OZ_ERROR( "%s: Item cannot have an inventory", name_ );
  }

  // default inventory
  if( nItems != 0 ) {
    const JSON& defaultItemsConfig = config["defaultItems"];
    int nDefaultItems = defaultItemsConfig.length();

    for( int i = 0; i < nDefaultItems; ++i ) {
      const char* itemName = defaultItemsConfig[i].get( "" );

      if( String::isEmpty( itemName ) ) {
        OZ_ERROR( "%s: Empty name for default item #%d", name_, i );
      }

      defaultItems.add( liber.objClass( itemName ) );
    }

    if( defaultItems.length() > nItems ) {
      OZ_ERROR( "%s: Too many items in the default inventory", name_ );
    }
  }

  /*
   * debris
   */

  const char* sFragPool = config["fragPool"].get( "" );

  nFrags   = config["nFrags"].get( 5 );
  fragPool = String::isEmpty( sFragPool ) ? nullptr : liber.fragPool( sFragPool );

  /*
   * device
   */

  deviceType = liber.deviceIndex( config["deviceType"].get( "" ) );

  if( deviceType >= 0 && ( flags & Object::USE_FUNC_BIT ) ) {
    OZ_ERROR( "%s: Device cannot have onUse handler", name_ );
  }

  /*
   * imago
   */

  imagoType  = liber.imagoIndex( config["imagoType"].get( "" ) );
  imagoModel = liber.modelIndex( config["imagoModel"].get( "" ) );

  if( imagoType >= 0 ) {
    flags |= Object::IMAGO_BIT;

    if( imagoModel < 0 ) {
      OZ_ERROR( "%s: invalid imagoModel", name_ );
    }
  }

  /*
   * audio
   */

  audioType = liber.audioIndex( config["audioType"].get( "" ) );
  aFill( audioSounds, MAX_SOUNDS, -1 );

  if( audioType >= 0 ) {
    flags |= Object::AUDIO_BIT;

    const JSON& soundsConfig = config["audioSounds"];

    const char* sEventCreate  = soundsConfig["create" ].get( "" );
    const char* sEventDestroy = soundsConfig["destroy"].get( "" );
    const char* sEventDamage  = soundsConfig["damage" ].get( "" );
    const char* sEventHit     = soundsConfig["hit"    ].get( "" );
    const char* sEventUse     = soundsConfig["use"    ].get( "" );
    const char* sEventFailed  = soundsConfig["failed" ].get( "" );

    audioSounds[Object::EVENT_CREATE]  = liber.soundIndex( sEventCreate  );
    audioSounds[Object::EVENT_DESTROY] = liber.soundIndex( sEventDestroy );
    audioSounds[Object::EVENT_DAMAGE]  = liber.soundIndex( sEventDamage  );
    audioSounds[Object::EVENT_HIT]     = liber.soundIndex( sEventHit     );
    audioSounds[Object::EVENT_USE]     = liber.soundIndex( sEventUse     );
    audioSounds[Object::EVENT_FAILED]  = liber.soundIndex( sEventFailed  );
  }

  /*
   * handlers
   */

  onDestroy = config["onDestroy"].get( "" );
  onUse     = config["onUse"].get( "" );
  onUpdate  = config["onUpdate"].get( "" );
  getStatus = config["getStatus"].get( "" );

  if( !onDestroy.isEmpty() ) {
    flags |= Object::LUA_BIT;

    // disable event handler if explicitly set to false
    if( !config["flag.onDestroy"].get( true ) ) {
      flags &= ~Object::DESTROY_FUNC_BIT;
    }
    else {
      flags |= Object::DESTROY_FUNC_BIT;
    }
  }
  if( !onUse.isEmpty() ) {
    flags |= Object::LUA_BIT;

    // disable event handler if explicitly set to false
    if( !config["flag.onUse"].get( true ) ) {
      flags &= ~Object::USE_FUNC_BIT;
    }
    else {
      flags |= Object::USE_FUNC_BIT;
    }
  }
  if( !onUpdate.isEmpty() ) {
    flags |= Object::LUA_BIT;

    // disable event handler if explicitly set to false
    if( !config["flag.onUpdate"].get( true ) ) {
      flags &= ~Object::UPDATE_FUNC_BIT;
    }
    else {
      flags |= Object::UPDATE_FUNC_BIT;
    }
  }
  if( !getStatus.isEmpty() ) {
    flags |= Object::LUA_BIT;

    // disable event handler if explicitly set to false
    if( !config["flag.getStatus"].get( true ) ) {
      flags &= ~Object::STATUS_FUNC_BIT;
    }
    else {
      flags |= Object::STATUS_FUNC_BIT;
    }
  }
}

Object* ObjectClass::create( int index, const Point& pos, Heading heading ) const
{
  return new Object( this, index, pos, heading );
}

Object* ObjectClass::create( InputStream* is ) const
{
  return new Object( this, is );
}

Object* ObjectClass::create( const JSON& json ) const
{
  return new Object( this, json );
}

}
