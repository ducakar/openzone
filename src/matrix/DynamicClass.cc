/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 * @file matrix/DynamicClass.cc
 */

#include "stable.hh"

#include "matrix/DynamicClass.hh"

#include "matrix/Dynamic.hh"
#include "matrix/Library.hh"

namespace oz
{
namespace matrix
{

ObjectClass* DynamicClass::createClass()
{
  return new DynamicClass();
}

void DynamicClass::initClass( const Config* config )
{
  flags = Object::DYNAMIC_BIT;

  OZ_CLASS_SET_FLAG( Object::DESTROY_FUNC_BIT,   "flag.onDestroy",     true  );
  OZ_CLASS_SET_FLAG( Object::DAMAGE_FUNC_BIT,    "flag.onDamage",      false );
  OZ_CLASS_SET_FLAG( Object::HIT_FUNC_BIT,       "flag.onHit",         false );
  OZ_CLASS_SET_FLAG( Object::USE_FUNC_BIT,       "flag.onUse",         false );
  OZ_CLASS_SET_FLAG( Object::UPDATE_FUNC_BIT,    "flag.onUpdate",      false );
  OZ_CLASS_SET_FLAG( Object::ITEM_BIT,           "flag.item",          false );
  OZ_CLASS_SET_FLAG( Object::SOLID_BIT,          "flag.solid",         true  );
  OZ_CLASS_SET_FLAG( Object::CYLINDER_BIT,       "flag.cylinder",      true  );
  OZ_CLASS_SET_FLAG( Object::PUSHER_BIT,         "flag.pusher",        false );
  OZ_CLASS_SET_FLAG( Object::NO_DRAW_BIT,        "flag.noDraw",        false );
  OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,      "flag.wideCull",      false );

  fillCommonConfig( config );

  if( audioType != -1 ) {
    const char* soundName;
    int         soundIndex;

    soundName  = config->get( "audioSound.splash", "" );
    soundIndex = String::isEmpty( soundName ) ? -1 : library.soundIndex( soundName );
    audioSounds[Object::EVENT_SPLASH] = soundIndex;

    soundName  = config->get( "audioSound.fricting", "" );
    soundIndex = String::isEmpty( soundName ) ? -1 : library.soundIndex( soundName );
    audioSounds[Object::EVENT_FRICTING] = soundIndex;
  }

  mass = config->get( "mass", 100.0f );
  lift = config->get( "lift", 12.0f );

  if( mass < 0.01f ) {
    throw Exception( "%s: Invalid object mass. Should be >= 0.01.", name.cstr() );
  }
  if( lift < 0.0f ) {
    throw Exception( "%s: Invalid object lift. Should be >= 0.", name.cstr() );
  }
}

Object* DynamicClass::create( int index, const Point3& pos, Heading heading ) const
{
  return new Dynamic( this, index, pos, heading );
}

Object* DynamicClass::create( InputStream* istream ) const
{
  return new Dynamic( this, istream );
}

}
}
