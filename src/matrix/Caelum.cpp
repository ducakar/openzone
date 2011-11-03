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
 * @file matrix/Caelum.cpp
 */

#include "stable.hpp"

#include "matrix/Caelum.hpp"

namespace oz
{
namespace matrix
{

Caelum::Caelum() : id( -1 ), heading( 0.0f ), period( 86400.0f ), time( 0.0f )
{}

void Caelum::update()
{
  time = Math::mod( time + Timer::TICK_TIME, period );
}

void Caelum::read( InputStream* istream )
{
  id      = istream->readInt();
  heading = istream->readFloat();
  period  = istream->readFloat();
  time    = istream->readFloat();
}

void Caelum::write( BufferStream* ostream ) const
{
  ostream->writeInt( id );
  ostream->writeFloat( heading );
  ostream->writeFloat( period );
  ostream->writeFloat( time );
}

}
}
