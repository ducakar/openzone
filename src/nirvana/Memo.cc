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
 * @file nirvana/Memo.cc
 */

#include <nirvana/Memo.hh>

namespace oz
{

Pool<Memo, 1024> Memo::pool;

Device* Memo::create( int, InputStream* istream )
{
  return new Memo( istream->readString() );
}

Memo::Memo( const char* text_ ) :
  Device( MEMO_BIT ), text( text_ )
{}

const char* Memo::type() const
{
  return "Memo";
}

const char* Memo::getMemo() const
{
  return text;
}

void Memo::write( OutputStream* ostream ) const
{
  ostream->writeString( text );
}

}
