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
 * @file matrix/Dynamic.cpp
 */

#include "stable.hpp"

#include "matrix/Dynamic.hpp"

namespace oz
{
namespace matrix
{

Pool<Dynamic, 4096> Dynamic::pool;

void Dynamic::readFull( InputStream* istream )
{
  Object::readFull( istream );

  parent     = istream->readInt();
  velocity   = istream->readVec3();
  momentum   = istream->readVec3();
  floor      = istream->readVec3();
  lower      = istream->readInt();
  depth      = istream->readFloat();
}

void Dynamic::writeFull( OutputStream* ostream ) const
{
  Object::writeFull( ostream );

  ostream->writeInt( parent );
  ostream->writeVec3( velocity );
  ostream->writeVec3( momentum );
  ostream->writeVec3( floor );
  ostream->writeInt( lower );
  ostream->writeFloat( depth );
}

void Dynamic::readUpdate( InputStream* istream )
{
  Object::readUpdate( istream );

  p        = istream->readPoint3();
  velocity = istream->readVec3();
  momentum = istream->readVec3();
}

void Dynamic::writeUpdate( OutputStream* ostream ) const
{
  Object::writeUpdate( ostream );

  ostream->writePoint3( p );
  ostream->writeFloat( life );
  ostream->writeVec3( velocity );
  ostream->writeVec3( momentum );
}

}
}
