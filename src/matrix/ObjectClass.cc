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

#include <stable.hh>
#include <matrix/ObjectClass.hh>

#include <common/Lingua.hh>
#include <matrix/Object.hh>
#include <matrix/Library.hh>

namespace oz
{
namespace matrix
{

ObjectClass::~ObjectClass()
{}

ObjectClass* ObjectClass::createClass()
{
  return new ObjectClass();
}

void ObjectClass::init( InputStream* is, const char* name_ )
{
  name        = name_;
  title       = lingua.get( is->readString() );
  description = lingua.get( is->readString() );

  dim         = is->readVec3();
  flags       = is->readInt();
  life        = is->readFloat();
  resistance  = is->readFloat();

  const char* sFragPool = is->readString();

  fragPool    = String::isEmpty( sFragPool ) ? null : library.fragPool( sFragPool );
  nFrags      = is->readInt();

  attributes  = is->readInt();
  key         = is->readInt();

  nItems      = is->readInt();

  int nDefaultItems = is->readInt();
  if( nDefaultItems != 0 ) {
    defaultItems.alloc( nDefaultItems );

    for( int i = 0; i < nDefaultItems; ++i ) {
      const char* sItemClass = is->readString();

      defaultItems.add( library.objClass( sItemClass ) );
    }
  }

  const char* sDeviceType = is->readString();
  deviceType = String::isEmpty( sDeviceType ) ? -1 : library.deviceIndex( sDeviceType );

  const char* sImagoType  = is->readString();
  const char* sImagoModel = is->readString();
  imagoType  = String::isEmpty( sImagoType ) ? -1 : library.imagoIndex( sImagoType );
  imagoModel = String::isEmpty( sImagoModel ) ? -1 : library.modelIndex( sImagoModel );

  const char* sAudioType = is->readString();
  audioType = String::isEmpty( sAudioType ) ? -1 : library.audioIndex( sAudioType );

  for( int i = 0; i < MAX_SOUNDS; ++i ) {
    const char* sSound = is->readString();

    audioSounds[i] = String::isEmpty( sSound ) ? -1 : library.soundIndex( sSound );
  }

  onDestroy = is->readString();
  onUse     = is->readString();
  onUpdate  = is->readString();
}

Object* ObjectClass::create( int index, const Point& pos, Heading heading ) const
{
  return new Object( this, index, pos, heading );
}

Object* ObjectClass::create( InputStream* istream ) const
{
  return new Object( this, istream );
}

}
}
