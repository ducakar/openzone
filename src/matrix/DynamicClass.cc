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
 * @file matrix/DynamicClass.cc
 */

#include <matrix/DynamicClass.hh>

#include <matrix/Dynamic.hh>

namespace oz
{
namespace matrix
{

ObjectClass* DynamicClass::createClass()
{
  return new DynamicClass();
}

void DynamicClass::init( InputStream* is, const char* name )
{
  ObjectClass::init( is, name );

  mass = is->readFloat();
  lift = is->readFloat();
}

Object* DynamicClass::create( int index, const Point& pos, Heading heading ) const
{
  return new Dynamic( this, index, pos, heading );
}

Object* DynamicClass::create( InputStream* istream ) const
{
  return new Dynamic( this, istream );
}

Object* DynamicClass::create( const JSON& json ) const
{
  return new Dynamic( this, json );
}

}
}
