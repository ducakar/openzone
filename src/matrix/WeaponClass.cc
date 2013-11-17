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
 * @file matrix/WeaponClass.cc
 */

#include <matrix/WeaponClass.hh>

#include <matrix/Weapon.hh>

namespace oz
{

ObjectClass* WeaponClass::createClass()
{
  return new WeaponClass();
}

void WeaponClass::init( InputStream* is, const char* name )
{
  DynamicClass::init( is, name );

  userBase     = is->readString();

  nRounds      = is->readInt();
  shotInterval = is->readFloat();

  onShot       = is->readString();
}

Object* WeaponClass::create( int index, const Point& pos, Heading heading ) const
{
  return new Weapon( this, index, pos, heading );
}

Object* WeaponClass::create( InputStream* is ) const
{
  return new Weapon( this, is );
}

Object* WeaponClass::create( const JSON& json ) const
{
  return new Weapon( this, json );
}

}
